#include "display.h"
#include "swap.h"
#include "triangle.h"

///////////////////////////////////////////////////////////////////////////////
// Return the normal vector of a triangle face
///////////////////////////////////////////////////////////////////////////////
void get_triangle_normal(vec4_t* vertices, vec3_t* face_normal) 
{
    // Get individual vectors from A, B, and C vertices to compute normal
    vec3_t vector_a = { {0} };
    vec3_from_vec4(&vertices[0], &vector_a); /*   A   */
    vec3_t vector_b = { {0} };
    vec3_from_vec4(&vertices[1], &vector_b); /*  / \  */
    vec3_t vector_c = { {0} };
    vec3_from_vec4(&vertices[2], &vector_c); /* C---B */

    // Get the vector subtraction of B-A and C-A
    vec3_t ba = { {0} };
    vec3_sub(&vector_b, &vector_a, &ba);
    vec3_t ca = { {0} };
    vec3_sub(&vector_c, &vector_a, &ca);
    vec3_normalize(&ba);
    vec3_normalize(&ca);

    // Compute the face normal (using cross product to find perpendicular)
    vec3_cross(&ba, &ca, face_normal);
    vec3_normalize(face_normal);
}

///////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights alpha, beta, and gamma for point p
///////////////////////////////////////////////////////////////////////////////
//
//          A
//         /|\
//        / | \
//       /  |  \
//      /  (p)  \
//     /  /   \  \
//    / /       \ \
//   //           \\
//  B ------------- C
//
///////////////////////////////////////////////////////////////////////////////
vec3_t barycentric_weights(vec2_t* a, vec2_t* b, vec2_t* c, vec2_t* p, vec3_t* r) {
    // Find the vectors between the vertices ABC and point p
    vec2_t ab = { {0} };
    vec2_sub(b, a, &ab);
    vec2_t bc = { {0} };
    vec2_sub(c, b, &bc);
    vec2_t ac = { {0} };
    vec2_sub(c, a, &ac);
    vec2_t ap = { {0} };
    vec2_sub(p, a, &ap);
    vec2_t bp = { {0} };
    vec2_sub(p, b, &bp);

    // Calcualte the area of the full triangle ABC using cross product (area of parallelogram)
    float area_triangle_abc = (ab.x * ac.y - ab.y * ac.x);

    // Weight alpha is the area of subtriangle BCP divided by the area of the full triangle ABC
    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;

    // Weight beta is the area of subtriangle ACP divided by the area of the full triangle ABC
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;

    // Weight gamma is easily found since barycentric cooordinates always add up to 1
    float gamma = 1 - alpha - beta;

    r->x = alpha;
    r->y = beta;
    r->z = gamma;
}

///////////////////////////////////////////////////////////////////////////////
// Draw a triangle using three raw line calls
///////////////////////////////////////////////////////////////////////////////
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw a solid pixel at position (x,y) using depth interpolation
///////////////////////////////////////////////////////////////////////////////
void draw_triangle_pixel(
    int x, int y,
    vec4_t* point_a, vec4_t* point_b, vec4_t* point_c,
    uint32_t color
) {
    // Create three vec2 to find the interpolation
    vec2_t p = { x, y };
    vec2_t a = { {0} };
    vec2_from_vec4(point_a, &a);
    vec2_t b = { {0} };
    vec2_from_vec4(point_b, &b);
    vec2_t c = { {0} };
    vec2_from_vec4(point_c, &c);

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    vec3_t weights = { {0} };
    barycentric_weights(&a, &b, &c, &p, &weights);

    // Fetch alpha, beta, and gamma from the barycentric coordinate calculation
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    
    // Interpolate the value of 1/w for the current pixel
    float interpolated_reciprocal_w = (1 / point_a->w) * alpha + (1 / point_b->w) * beta + (1 / point_c->w) * gamma;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)){
        // Draw a pixel at position (x,y) with a solid color
        draw_pixel(x, y, color);

        // Update the z-buffer value with the 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at position (x,y) using depth interpolation
///////////////////////////////////////////////////////////////////////////////
void draw_triangle_texel(
    int x, int y,
    vec4_t* point_a, vec4_t* point_b, vec4_t* point_c,
    tex2_t* a_uv, tex2_t* b_uv, tex2_t* c_uv,
    upng_t* texture
) {
    vec2_t p = { x, y };
    vec2_t a = { {0} };
    vec2_from_vec4(point_a, &a);
    vec2_t b = { {0} };
    vec2_from_vec4(point_b, &b);
    vec2_t c = { {0} }; 
    vec2_from_vec4(point_c, &c);

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    vec3_t weights = barycentric_weights(a, b, c, p);

    // Fetch alpha, beta, and gamma from the barycentric coordinates calculation
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    
    // Variables to store the interpolated values of U, V, and also 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
    interpolated_u = (a_uv->u / point_a->w) * alpha + (b_uv->u / point_b->w) * beta + (c_uv->u / point_c->w) * gamma;
    interpolated_v = (a_uv->v / point_a->w) * alpha + (b_uv->v / point_b->w) * beta + (c_uv->v / point_c->w) * gamma;

    // Also interpolate the value of 1/w for the current pixel
    interpolated_reciprocal_w = (1 / point_a->w) * alpha + (1 / point_b->w) * beta + (1 / point_c->w) * gamma;

    // Now we can divide back both interpolated values by 1/w
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Get the mesh texture width and height dimensions
    int texture_width = upng_get_width(texture);
    int texture_height = upng_get_height(texture);

    // Map the UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        // Draw a pixel at position (x,y) with the color that comes from the mapped texture
        uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
        draw_pixel(x, y, texture_buffer[(texture_width * tex_y) + tex_x]);

        // Update the z-buffer value with the 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    upng_t* texture
) {
    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // Flip the V component to account for inverted UV-coordinates (V grows downwards)
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    // Create vector points and texture coords after we sort the vertices
    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };
    tex2_t a_uv = { u0, v0 };
    tex2_t b_uv = { u1, v1 };
    tex2_t c_uv = { u2, v2 };

    ///////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom)
    ///////////////////////////////////////////////////////
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_triangle_texel(x, y, &point_a, &point_b, &point_c, &a_uv, &b_uv, &c_uv, texture);
            }
        }
    }

    ///////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top)
    ///////////////////////////////////////////////////////
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_triangle_texel(x, y, &point_a, &point_b, &point_c, &a_uv, &b_uv, &c_uv, texture);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)---------\
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void draw_filled_triangle(
    int x0, int y0, float z0, float w0,
    int x1, int y1, float z1, float w1,
    int x2, int y2, float z2, float w2,
    uint32_t color
) {
    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    // Create three vector points after we sort the vertices
    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };

    ///////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom)
    ///////////////////////////////////////////////////////
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
                draw_triangle_pixel(x, y, &point_a, &point_b, &point_c, color);
            }
        }
    }

    ///////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top)
    ///////////////////////////////////////////////////////
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
                draw_triangle_pixel(x, y, &point_a, &point_b, &point_c, color);
            }
        }
    }
}
