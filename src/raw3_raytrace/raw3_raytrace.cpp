// Example raw3_raytrace
// - Simple demonstration of raytracing/pathtracing without any acceleration techniques
// - Casts rays from camera space into scene and recursively traces reflections/refractions
// - Materials are extended to support simple specular reflections and transparency with refraction index

#include <iostream>
#include <ppgso/ppgso.h>

using namespace std;
using namespace glm;
using namespace ppgso;

// Global constants
constexpr double INF = numeric_limits<double>::max();       // Will be used for infinity
constexpr double EPS = numeric_limits<double>::epsilon();   // Numerical epsilon
const double DELTA = sqrt(EPS);                             // Delta to use

/*!
 * Structure holding origin and direction that represents a ray
 */
struct Ray {
  dvec3 origin, direction;

  /*!
   * Compute a point on the ray
   * @param t Distance from origin
   * @return Point on ray where t is the distance from the origin
   */
  inline dvec3 point(double t) const {
    return origin + direction * t;
  }
};

/*!
 * Material coefficients for diffuse and emission
 */
struct Material {
  dvec3 emission, diffuse;
  double reflectivity;
  double transparency, refractionIndex;
};

/*!
 * Structure to represent a ray to object collision, the Hit structure will contain material surface normal
 */
struct Hit {
  double distance;
  dvec3 point, normal;
  Material material;
};

/*!
 * Constant for collisions that have not hit any object in the scene
 */
const Hit noHit{ INF, {0,0,0}, {0,0,0}, { {0,0,0}, {0,0,0}, 0, 0, 0 } };

/*!
 * Structure representing a simple camera that is composed on position, up, back and right vectors
 */
struct Camera {
  dvec3 position, back, up, right;

  /*!
   * Generate a new Ray for the given viewport size and position
   * @param x Horizontal position in the viewport
   * @param y Vertical position in the viewport
   * @param width Width of the viewport
   * @param height Height of the viewport
   * @return Ray for the giver viewport position with small random deviation applied to support multi-sampling
   */
  Ray generateRay(int x, int y, int width, int height) const {
    // Camera deltas
    dvec3 vdu = 2.0 * right / (double)width;
    dvec3 vdv = 2.0 * -up / (double)height;

    Ray ray;
    ray.origin = position;
    ray.direction = -back
                  + vdu * ((double)(-width/2 + x) + linearRand(0.0, 1.0))
                  + vdv * ((double)(-height/2 + y) + linearRand(0.0, 1.0));
    ray.direction = normalize(ray.direction);
    return ray;
  }
};

/*!
 * Structure representing a sphere which is defined by its center position, radius and material
 */
struct Sphere {
  double radius;
  dvec3 center;
  Material material;

  /*!
   * Compute ray to sphere collision
   * @param ray Ray to compute collision against
   * @return Hit structure that represents the collision or noHit.
   */
  inline Hit hit(const Ray &ray) const {
    dvec3 oc = ray.origin - center;
    double a = dot(ray.direction, ray.direction);
    double b = dot(oc, ray.direction);
    double c = dot(oc, oc) - radius * radius;
    double dis = b * b - a * c;

    if (dis > 0) {
      double e = sqrt(dis);
      double t = (-b - e) / a;

      if ( t > EPS ) {
        dvec3 pt = ray.point(t);
        dvec3 n = normalize(pt - center);
        return {t, pt, n, material};
      }

      t = (-b + e) / a;

      if ( t > EPS ) {
        dvec3 pt = ray.point(t);
        dvec3 n = normalize(pt - center);
        return {t, pt, n, material};
      }
    }
    return noHit;
  }
};

/*!
 * Generate a normalized vector that sits on the surface of a half-sphere which is defined using a normal. Used to generate random diffuse reflections.
 * @param normal Normal that defines the dome/half-sphere direction
 * @return Random 3D vector on the dome surface
 */
inline dvec3 RandomDome(const dvec3 &normal) {
  double d;
  dvec3 p;

  do {
    p = sphericalRand(1.0);
    d = dot(p, normal);
  } while(d < 0);

  return p;
}

/*!
 * Structure to represent the scene/world to render
 */
struct World {
  Camera camera;
  vector<Sphere> spheres;

  /*!
   * Compute ray to object collision with any object in the world
   * @param ray Ray to trace collisions for
   * @return Hit or noHit structure which indicates the material and distance the ray has collided with
   */
  inline Hit cast(const Ray &ray) const {
    Hit hit = noHit;
    for (auto &sphere : spheres) {
      auto lh = sphere.hit(ray);

      if (lh.distance < hit.distance) {
        hit = lh;
      }
    }
    return hit;
  }

  /*!
   * Trace a ray as it collides with objects in the world
   * @param ray Ray to trace
   * @param depth Maximum number of collisions to trace
   * @return Color representing the accumulated lighting for each ray collision
   */
  inline dvec3 trace(const Ray &ray, unsigned int depth) const {
    if (depth == 0) return {0, 0, 0};

    const Hit hit = cast(ray);

    // No hit
    if ( std::isinf(hit.distance)) return {0, 0, 0};

    // Emission
    dvec3 color = hit.material.emission;

    // Decide to reflect or refract using linear random
    if (linearRand(0.0f, 1.0f) < hit.material.transparency) {
      // Flip normal if the ray is "inside" a sphere
      dvec3 normal = dot(ray.direction, hit.normal) < 0 ? hit.normal : -hit.normal;
      // Reverse the refraction index as well
      double r_index = dot(ray.direction, hit.normal) < 0 ? 1/hit.material.refractionIndex : hit.material.refractionIndex;

      // Prepare refraction ray
      dvec3 refraction = refract(ray.direction, normal, r_index);
      Ray refractionRay{hit.point - normal * DELTA, refraction};
      // Modulate the refraction color with diffuse color
      dvec3 refractionColor = lerp(hit.material.diffuse, {1,1,1}, hit.material.transparency);
      // Trace the ray recursively
      color += refractionColor * trace(refractionRay, depth - 1);
    } else {
      // Calculate reflection
      // Random diffuse reflection
      dvec3 diffuse = RandomDome(hit.normal);
      // Ideal specular reflection
      dvec3 reflection = reflect(ray.direction, hit.normal);
      // Ray that combines reflection direction depending on the material reflectivness
      Ray reflectedRay{hit.point + hit.normal * DELTA, lerp(diffuse, reflection, hit.material.reflectivity)};
      // Reflection color is white for specular reflections, otherwise diffuse color is used
      dvec3 reflectionColor = lerp(hit.material.diffuse, {1, 1, 1}, hit.material.reflectivity);
      // Trace the ray recursively
      color += reflectionColor * trace(reflectedRay, depth - 1);
    }

    return color;
  }

  /*!
   * Render the world to the provided image
   * @param image Image to render to
   */
  void render(Image& image, unsigned int samples, unsigned int depth) const {
    // For each pixel generate rays
    #pragma omp parallel for
    for (int y = 0; y < image.height; ++y) {
      for (int x = 0; x < image.width; ++x) {
        dvec3 color{};

        // Generate multiple samples
        for (unsigned int i = 0; i < samples; ++i) {
          auto ray = camera.generateRay(x, y, image.width, image.height);
          color = color + trace(ray, depth);
        }
        // Collect the data
        color = color / (double) samples;
        image.setPixel(x, y, (float)color.r, (float)color.g, (float)color.b);
      }
    }
  }
};

int main() {
  cout << "This will take a while ..." << endl;

  // Image to render to
  Image image{512, 512};

  // World to render
  const World world{
      { // Camera
          {  0,   0, 25}, // Position
          {  0,   0,  1}, // Back
          {  0,  .5,  0}, // Up
          { .5,   0,  0}, // Right
      },
      { // Spheres
          { 10000, {  0, -10010, 0}, { {0, 0, 0}, {.8, .8, .8}, 0, 0, 0 } },        // Floor
          { 10000, { -10010, 0, 0}, { { 0, 0, 0}, { 1, 0, 0}, 0, 0, 0 } },          // Left wall
          { 10000, {  10010, 0, 0}, { { 0, 0, 0}, { 0, 1, 0}, 0, 0, 0 } },          // Right wall
          { 10000, {  0,0, -10010}, { { 0, 0, 0}, { .8, .8, 0}, 0, 0, 0 } },        // Back wall
          { 10000, {  0,0, 10030}, { { 0, 0, 0}, { 0, .8, .8}, 0, 0, 0 } },         // Front wall (behind camera)
          { 10000, {  0,10010, 0}, { { 1, 1, 1}, { .8, .8, .8}, 0, 0, 0 } },        // Ceiling and source of light
          {     2, { -5,  -8,  3}, { { 0, 0, 0}, { .7, .7, 0}, 1, .95, 1.52 } },    // Refractive glass sphere
          {     4, {  0,  -6,  0}, { { 0, 0, 0}, { .7, .5, .1}, 1, 0, 0 } },        // Reflective sphere
          {    10, {  10, 10, -10}, { { 0, 0, 0}, { 0, 0, 1}, 0, 0, 1.54 } },       // Sphere in top right corner
      },
  };

  // Render the scene
  world.render(image, 32, 5);

  // Save the result
  image::saveBMP(image, "raw3_raytrace.bmp");

  cout << "Done." << endl;
  return EXIT_SUCCESS;
}
