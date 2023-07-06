// ====================================================
#version 330 core

// ================== Global variables ================
#define PI 3.14159265358f

#define TYPE_SPHERE 0
#define TYPE_POINT_LIGHT 1

#define SHADOW_ENABLED 1

#define TOTAL_INTERNAL_REFLECTION 1
#define DO_FRESNEL 1
#define REFLECT_REDUCE_ITERATION 1

#define DBG 0

// =================== Defining structs ==================

struct rt_material {
    vec3 color;
    vec3 absorb;

    float diffuse;
    float reflection;
    float refraction;
    int specular;
    float kd;
    float ks;
};

struct rt_sphere {
    rt_material mat;
    vec4 obj; // center coords and radius
    vec4 quat_rotation; // rotat normal
    int textureNum;
    bool hollow;
};

struct rt_light_direct {
    vec3 direction;
    vec3 color;
    float intensity;
};

struct rt_light_point {
    vec4 pos; // pos + radius
    vec3 color;
    float intensity;

    float linear_k;
    float quadratic_k;
};

struct rt_scene {
    vec4 quat_camera_rotation;
    vec3 camera_pos;
    vec3 bg_color;

    int width;
    int height;

    int reflect_depth;
};

struct hit_record {
    rt_material mat;
    vec3 normal;
    float bias_mult;
    float alpha;
};

#define SPHERE_SIZE {SPHERE_SIZE}
#define LIGHT_DIRECT_SIZE {LIGHT_DIRECT_SIZE}
#define LIGHT_POINT_SIZE {LIGHT_POINT_SIZE}
#define AMBIENT_COLOR {AMBIENT_COLOR}
#define SHADOW_AMBIENT {SHADOW_AMBIENT}
#define ITERATIONS {ITERATIONS}

out vec4 FragColor;

uniform samplerCube background;

const float maxDist = 1000000.0;

vec3 opt_normal;
vec2 opt_uv;

#if DBG
bool dbgEd = false;
#endif

layout(std140) uniform scene_buf{
    rt_scene scene;
};

layout(std140) uniform spheres_buf{
    #if SPHERE_SIZE != 0
        rt_sphere spheres[SPHERE_SIZE];
    #else
        rt_sphere spheres[1];
    #endif
};

layout(std140) uniform lights_point_buf{
    #if LIGHT_POINT_SIZE != 0
        rt_light_point lights_point[LIGHT_POINT_SIZE];
    #else
        rt_light_point lights_point[1];
    #endif
};

layout(std140) uniform lights_direct_buf{
    #if LIGHT_DIRECT_SIZE != 0
        rt_light_direct lights_direct[LIGHT_DIRECT_SIZE];
    #else
        rt_light_direct lights_direct[1];
    #endif
};


void _dbg()
{
	#if DBG
	#if DBG_FIRST_VALUE 
	if (dbgEd)
		return;
	#endif
	
	ivec2 pixel_coords = ivec2 (gl_FragCoord.xy);
    FragColor =vec4(1,0,0,1);
	dbgEd = true;
	#endif
}

void _dbg(float value)
{
	#if DBG
	#if DBG_FIRST_VALUE 
	if (dbgEd)
		return;
	#endif
	value = clamp(value, 0, 1);
	ivec2 pixel_coords = ivec2 (gl_FragCoord.xy);
    FragColor = vec4(value,value,value,1);
	dbgEd = true;
	#endif
}

void _dbg(vec3 value)
{
	#if DBG
	#if DBG_FIRST_VALUE 
	if (dbgEd)
		return;
	#endif
	ivec2 pixel_coords = ivec2 (gl_FragCoord.xy);
    FragColor = vec4(clamp(value, vec3(0), vec3(1)),1);
	dbgEd = true;
	#endif
}

vec4 quaternion_conjugate(vec4 q){
    return vec4(-q.x, -q.y, -q.z, q.w);
}

vec4 quaternion_inv(vec4 q){
    return quaternion_conjugate(q) * (1/dot(q,q));
}

vec4 quaternion_mult(vec4 q1, vec4 q2){
    vec4 qr;
    qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
	qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
	qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
	qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
	return qr;
}

vec3 rotate(vec4 qr, vec3 v){
    vec4 qr_conj = quaternion_conjugate(qr);
    vec4 q_pos = vec4(v.xyz, 0);
    vec4 q_tmp = quaternion_mult(qr, q_pos);
    return quaternion_mult(q_tmp, qr_conj).xyz; 
}

// ================== Computing functions for intersection ======================

vec3 getRayDir(){
    // Direction from center of box to fragment xy coords
    vec2 directionXY = (gl_FragCoord.xy - vec2(scene.width, scene.height) / 2) / scene.height;
    vec3 result = vec3(directionXY, 1);
    // Setup ray direction looking at camera
    return normalize(rotate(scene.quat_camera_rotation, result));
}

bool intersectSphere(vec3 rayOrigin, vec3 rayDirection, vec4 sphereObj, bool hollow, float tmin, out float t){
    vec3 objectCenter = rayOrigin - sphereObj.xyz;
    float b = dot(objectCenter, rayDirection);
    float c = dot(objectCenter, objectCenter) -  sphereObj.w * sphereObj.w;
    float h = b*b - c;
    if (h < 0.0){
        return false;
    }
    float h_sqrt = sqrt(h);
    t = - b - h_sqrt;
    if (hollow && t < 0.0){
        t = -b + h_sqrt;
    }
    return t > 0 && t < tmin;
}

// Function to compute the distance (t value) to first intersected object in ray path
float calcIntersections(vec3 rayOrigin, vec3 rayDirection, out int num, out int type){
    float tmin = maxDist;
    float tmp_t;
    for (int i=0; i < SPHERE_SIZE; i++){
        if (intersectSphere(rayOrigin, rayDirection, spheres[i].obj, spheres[i].hollow, tmin, tmp_t)){
            num = i;
            tmin = tmp_t;
            type = TYPE_SPHERE;
        }
    }
    for (int i=0; i<LIGHT_POINT_SIZE; i++){
        if (intersectSphere(rayOrigin, rayDirection, lights_point[i].pos, false, tmin, tmp_t)){
            num = i;
            tmin = tmp_t;
            type = TYPE_POINT_LIGHT;
        }
    }

    return tmin;
}

hit_record get_hit_info(vec3 rayOrigin, vec3 rayDirection, vec3 pointInt, float t, int num, int type){
    hit_record hr;
    if (type == TYPE_SPHERE){
        rt_sphere sphere = spheres[num];
        hr = hit_record(sphere.mat, normalize(pointInt - sphere.obj.xyz), 0, 1);
        // Code for available texture would go here
    }
    float distance = length(pointInt - rayOrigin);
    hr.bias_mult = (9e-3 * distance + 35) / 35e3;
    return hr;
}

// Functions to compute shadows and shades
float inShadow(vec3 rayOrigin, vec3 rayDirection, float dist){
    float t;
    float shadow = 0;

    for (int i=0; i < SPHERE_SIZE; i++){
        if (intersectSphere(rayOrigin, rayDirection, spheres[i].obj, false, dist, t)){
            shadow = 1;
        }
    }
    return min(shadow, 1);
}

void calcShadeSpecularDiffusion(vec3 light_dir, vec3 light_color, float intensity, vec3 pointInt, vec3 rayDirection, rt_material material, vec3 normal, bool doShadow, float dist, float distDiv, inout vec3 diffuse, inout vec3 specular){
    light_dir = normalize(light_dir);
    // diffuse
    float dp = clamp(dot(normal, light_dir), 0.0, 1.0);
    light_color *= dp;
    #if SHADOW_ENABLED
    if (doShadow) {
        vec3 shadow = vec3(1 - inShadow(pointInt, light_dir, dist));
        light_color *= max(shadow, SHADOW_AMBIENT);
    }
    #endif
    diffuse += light_color * material.color * material.diffuse * intensity / distDiv;

    //specular
    if (material.specular > 0){
        vec3 reflection = reflect(light_dir, normal);
        float specDp = clamp(dot(rayDirection, reflection), 0.0, 1.0);
        specular += light_color * pow(specDp, material.specular) * intensity / distDiv;
    }

}

vec3 calcShadeMaterial(vec3 pointInt, vec3 rayDirection, rt_material material, vec3 normal, bool doShadow){
    float dist, distDiv;
    vec3 light_color, light_dir;
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);

    vec3 pixelColor = AMBIENT_COLOR * material.color;

    for (int i=0; i < LIGHT_POINT_SIZE; i++){
        rt_light_point light = lights_point[i];
        light_color = light.color;
        light_dir = light.pos.xyz - pointInt;
        dist = length(light_dir);
        distDiv = 1 + light.linear_k * dist + light.quadratic_k * dist * dist;
        calcShadeSpecularDiffusion(light_dir, light_color, light.intensity, pointInt, rayDirection, material, normal, doShadow, dist, distDiv, diffuse, specular);
    }
    for (int i=0; i<LIGHT_DIRECT_SIZE; i++){
        light_color = lights_direct[i].color;
        light_dir = -lights_direct[i].direction;
        dist = maxDist;
        distDiv = 1;
        calcShadeSpecularDiffusion(light_dir, light_color, lights_direct[i].intensity, pointInt, rayDirection, material, normal, doShadow, dist, distDiv, diffuse, specular);
    }

    pixelColor += diffuse * material.kd + specular * material.ks;
    return pixelColor;
}

// Function to compute reflected color
vec3 getReflectedColor(vec3 rayOrigin, vec3 rayDirection){
    vec3 color = vec3(0);
    vec3 pointInt;
    int num, type;
    float t = calcIntersections(rayOrigin, rayDirection, num, type);
    if (type == TYPE_POINT_LIGHT) {
        return lights_point[num].color;
    }
    hit_record hr;
    if (t < maxDist){
        pointInt = rayOrigin + rayDirection * t;
        hr = get_hit_info(rayOrigin, rayDirection, pointInt, t, num, type);
        rayOrigin = dot(rayDirection, hr.normal) < 0 ? pointInt + hr.normal * hr.bias_mult : pointInt - hr.normal * hr.bias_mult;
        color = calcShadeMaterial(rayOrigin, rayDirection, hr.mat, hr.normal, true);
    }
    return color;
}

// =============== Functions for Fresnel =================

float getFresnel(vec3 normal, vec3 rayDirection, float reflection){
    // Computing for Schlick's approximation
    float ndotv = clamp(dot(normal, -rayDirection), 0.0, 1.0);
    return reflection + (1.0 - reflection) * pow(1.0 - ndotv, 5.0);
}

float FresnelReflectAmount(float normal1, float normal2, vec3 normal, vec3 incident, float reflection){
    #if DO_FRESNEL
        // Schlick approximation
        float r0 = (normal1 - normal2) / (normal1 + normal2);
        r0 *= r0;
        float cosX = -dot(normal, incident);
        if (normal1 > normal2){
            float n = normal1/normal2;
            float sinT2 = n*n*(1.0 - cosX*cosX);
            // Total internal reflection
            if (sinT2 > 1.0){
                return 1.0;
            }
            cosX = sqrt(1.0 - sinT2);
        }
        float x = 1.0 - cosX;
        float ret = r0 + (1.0 - r0)*x*x*x*x*x;
        ret  = (reflection + (1.0-reflection) * ret);
        return ret;
    #else
        return reflection;
    #endif
}

// =============== Main functions ========================

void main() {
    float reflectMultiplier, refractMultiplier, t;
    vec3 col;
    rt_material mat;
    vec3 pointInt, n; // pointInt is point of intersection and n is normal
    vec4 ob; // Object specification

    vec3 mask = vec3(1.0);
    vec3 color = vec3(0.0);
    vec3 rayOrigin = vec3(scene.camera_pos); // Ray origin
    vec3 rayDirection = getRayDir(); // Ray Direction
    float absorbDistance = 0.0;
    int type = 0; // Object type ray finds
    int num;
    hit_record hr;

    for (int i = 0; i < ITERATIONS; i++){
        t = calcIntersections(rayOrigin, rayDirection, num, type);
        if (t < maxDist){
            pointInt = rayOrigin + rayDirection * t; // Point of ray and object intersection
            hr = get_hit_info(rayOrigin, rayDirection, pointInt, t, num, type);
            
            if (type == TYPE_POINT_LIGHT){
                color += lights_point[num].color * mask;
                break;
            }

            mat = hr.mat; // Hit record material
            n = hr.normal; // Hit record normal

            // Deciding if normal should go inside or outside of object
            bool outside = dot(rayDirection, n) < 0;
            n = outside ? n : -n; 

            #if TOTAL_INTERNAL_REFLECTION
            if (mat.refraction > 0){
                reflectMultiplier = FresnelReflectAmount( outside ? 1 : mat.refraction,
                                                          outside ? mat.refraction : 1,
                                                          rayDirection, n, mat.reflection);
            } else {
                reflectMultiplier = getFresnel(n, rayDirection, mat.reflection);
            }
            #else
            reflectMultiplier = getFresnel(n, rayDirection, mat.reflection);
            #endif
            refractMultiplier = 1 - reflectMultiplier;

            // Do specific computations depending on material
            if (mat.refraction > 0.0) {
                if (outside && mat.reflection > 0) {
                    color += getReflectedColor(pointInt + n*hr.bias_mult, reflect(rayDirection, n)) * reflectMultiplier * mask;
                    mask *= refractMultiplier;
                } else if (!outside) {
                    absorbDistance += t;
                    vec3 absorb = exp(-mat.absorb * absorbDistance);
                    mask *= absorb;
                }
                #if TOTAL_INTERNAL_REFLECTION
				if (reflectMultiplier >= 1)
					break;
				#endif
                rayOrigin = pointInt - n * hr.bias_mult;
                rayDirection = refract(rayDirection, n, outside ? 1 / mat.refraction : mat.refraction);
                #ifdef REFLECT_REDUCE_ITERATION
                    i--;
                #endif
            } else if(mat.reflection > 0.0) { // Refective 
                rayOrigin = pointInt + n * hr.bias_mult;
                color += calcShadeMaterial(rayOrigin, rayDirection, mat, n, true) * refractMultiplier * mask;
                rayDirection = reflect(rayDirection, n);
                mask *= reflectMultiplier;
            } else { // Diffuse
                color += calcShadeMaterial(pointInt + n*hr.bias_mult, rayDirection, mat, n, true) * mask * hr.alpha;
                if (hr.alpha < 1) {
                    rayOrigin = pointInt - n * hr.bias_mult;
                    mask *= 1 - hr.alpha;
                } else {
                    break;
                }
            }
        } else {
            color += texture(background, rayDirection).rgb * mask;
            break;
        }
    }
    #if DBG == 0
	FragColor = vec4(color,1);
	#else
	if (!dbgEd) FragColor = vec4(color,1);
	#endif
}
