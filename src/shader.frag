#version 330 core

#define Point   0
#define Sphere  1
#define AACube  2

#define EPSILON 0.01
#define MAX_OBJECTS 20

#define ANGLE_SHADING
#define DISTANCE_SHADING
#define AMBIENT_OCCLUSION

struct Object
{
     int type;
     vec3 position;
     float size;
};

struct Camera
{
    vec3 position;
    vec3 direction;
};

out vec4 diffuseColor;
uniform int num_objects;
uniform Object objects[MAX_OBJECTS];
uniform Camera camera;
uniform vec2 resolution;

float dist(vec3 origin, int i) {
    if (objects[i].type == Point) {
        return distance(origin, objects[i].position); 
    } else if (objects[i].type == Sphere) {
        return distance(origin, objects[i].position) - objects[i].size;
    } else if (objects[i].type == AACube) {
        vec3 d = abs(origin - objects[i].position);
        float size = objects[i].size / 2;
        if (d.x <= size && d.y <= size && d.z <= size) {
            // Inside - always a face
            return -min(min(size-d.x, size-d.y), size-d.z);
        } else if (d.x > size && d.y > size && d.z > size) {
            // Corner
            return length(d - vec3(size));
        } else {
            float smallest = min(min(d.x, d.y), d.z);
            float largest = max(max(d.x, d.y), d.z);
            float middle = d.x + d.y + d.z - smallest - largest;
            // Note that smallest is always less than size
            if (middle <= size) {
                // Face
                return largest - size;
            } else {
                // Edge
                return length(vec2(middle, largest) - vec2(size));
            }
        }
    } else {
        return distance(origin, objects[i].position); 
    }
}

vec3 norm(vec3 origin, int i) {
    if (objects[i].type == Point) {
        return normalize(origin - objects[i].position);
    } else if (objects[i].type == Sphere) {
        return normalize(origin - objects[i].position);
    } else if (objects[i].type == AACube) {
        vec3 d = origin - objects[i].position;
        float size = objects[i].size / 2;
        if (abs(d.x) <= size) {
            d.x = 0.0;
        }
        if (abs(d.y) <= size) {
            d.y = 0.0;
        }
        if (abs(d.z) <= size) {
            d.z = 0.0;
        }
        return normalize(d);
    } else {
        return normalize(origin - objects[i].position);
    }
}

void main() {
    float ratio = resolution.y/resolution.x;
    // ([-1,1], [-1,1])
    vec2 coord = 2*gl_FragCoord.xy/resolution - vec2(1.0);
    // ([-1,1], [-x,x]:x<1)
    coord.y *= ratio;

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, camera.direction));
    up *= coord.y;
    right *= coord.x;
    vec3 direction = normalize(up + right + camera.direction);
    //vec3 direction = normalize(vec3(coord, 1.0) + camera.direction);
    vec3 origin = camera.position;

    float distances[MAX_OBJECTS];

    // Max steps: 100
    int max_steps = 1000;
    // Falloff distance: 100
    float max_dist = 10.0;
    float min_dist = max_dist;
    int steps = 0;
    int lasthit = 0;
    bool decrease = false;
    decrease = !decrease;
    bool hit = false;
    vec3 raypos = origin;
    for (steps = 0; steps < max_steps; steps++) {
        decrease = false;
        for (int i = 0; i < num_objects; i++) {
            float i_dist = dist(raypos, i);
            if (steps == 0) {
                distances[i] = i_dist;
                decrease = true;
            } else if (i_dist <= distances[i]) {
                decrease = true;
            }
            if (i_dist < min_dist)
                lasthit = i;
            min_dist = min(min_dist, i_dist);
        }

        hit = min_dist < EPSILON;

        if (!decrease || hit) {
            break;
        }

        vec3 pos1 = raypos;
        raypos += min_dist * direction;
    }

    float f_steps = float(steps) + 1;
    if (hit) {
        vec3 hitvec = norm(raypos, lasthit);
        diffuseColor = vec4(1.0);
#ifdef ANGLE_SHADING
        // Angle shading
        diffuseColor.xyz = vec3(abs(dot(direction, hitvec)));
#endif
#ifdef DISTANCE_SHADING
        // Distance shading
        diffuseColor.xyz /= 2;
        diffuseColor.xyz += vec3(1.0);
        diffuseColor.xyz -= vec3(distance(origin, raypos)/max_dist);
#endif
#ifdef AMBIENT_OCCLUSION
        // Ambient Occlusion
        diffuseColor.xyz -= vec3(f_steps/float(max_steps));
#endif
    } else if (!decrease) {
        // Didn't hit anything - use steps
        diffuseColor = vec4(vec3(f_steps/float(max_steps)), 1.0);
    } else {
        diffuseColor = vec4(vec3(0.0), 1.0);
    }
}

