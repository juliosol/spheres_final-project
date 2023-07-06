#version 330 core

// Final output color of each 'fragment' from fragment shader
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct ray {
    vec3 origin;
    vec3 direction;
};

struct hit_record {
    vec3 p;
    vec3 normal;
    float t;

    bool front_face;

    void set_face_normal(const ray r, const vec3 outward_normal) {
        front_face = dot(r.direction, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

struct Sphere {
    float centerX;
    float centerY;
    float centerZ;
    vec3 center;
    int radius;
    
    hit_record sphereRecord;
};





// In data
in vec3 myNormal; // Import normal data
in vec2 v_texCoord; // Import texture coordinates from vertex shader
in vec3 FragPos; // Import fragment position

uniform vec3 sphereCenter;
uniform float sphereRadius;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

float specularStrength = 0.5f;

// Helper function for vectors

float length_squared(vec3 inputVec){
    return inputVec.x*inputVec.x + inputVec.y * inputVec.y + inputVec.z * inputVec.z;
}

float length(vec3 inputVec){
    return sqrt(length_squared(inputVec));
}

float dot(vec3 a, vec3 b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot2(vec2 a, vec2 b){
    return a.x * b.x + a.y * b.y;
}

vec3 unitVector(vec3 v){
    return v / length(v);
}

vec3 rayAt(ray r, float t){
    return r.origin + t * r.direction;
}

float rand(vec2 co){
  return fract(sin(dot2(co.xy , vec2(12.9898,78.233))) * 43758.5453);
}

float random_double(){
    return rand(vec2(0.8, 0.39));
}

float random_double(float min, float max) {
    // Returns a random real in [min, max).
    return min + (max-min)*rand(vec2(0.7,0.6));
}

vec3 random() {
    return vec3(random_double(), random_double(), random_double());
}

vec3 random(float min, float max) {
    return vec3(random_double(min,max), random_double(min,max), random_double(min,max));
}

vec3 random_in_unit_sphere() {
    while (true) {
        vec3 p = random(-1,1);
        if (length_squared(p) >= 1) continue;
        return p;
    }
}

// Functions for computing if ray hits sphere

bool hit_sphere(Sphere s, const ray r, float t_min, float t_max) {
    vec3 oc = vec3(0.0, 0.0, 0.0) - s.center;
    float a = length_squared(r.direction);
    float half_b = dot(oc, r.direction);
    float c = length_squared(oc) - s.radius*s.radius;
    float discriminant =  half_b*half_b - a*c;
    if (discriminant < 0){
        return false;
    } 

    float sqrtd = sqrt(discriminant);

    // Find nearerst root that lies in the acceptable range
    float root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root){
            return false;
        }
    }

    s.sphereRecord.t = root;
    s.sphereRecord.p = rayAt(r, s.sphereRecord.t);
    vec3 outward_normal = (s.sphereRecord.p - s.center) / s.radius;
    s.sphereRecord.set_face_normal(r, outward_normal);

    return true;
}

vec3 rayColor(ray r, Sphere s, int depth) {

    if (depth <= 0)
        return vec3(0.0,0.0,0.0);

    if (hit_sphere(s, r, 0, 1000000)) {
        //vec3 N = unitVector(rayAt(r, t) - vec3(0.0, 0.0,-1.0));
        //return 0.5 * vec3(N.x + 1.0, N.y + 1.0, N.z + 1.0);
        vec3 target = s.sphereRecord.p + s.sphereRecord.normal;// + random_in_unit_sphere();
        ray tmpRay;
        tmpRay.origin = s.sphereRecord.p;
        tmpRay.direction = target - s.sphereRecord.p;
        return 0.5 * rayColor(tmpRay, s, depth-1);
        //return 0.5 * (s.sphereRecord.normal + vec3(1.0,1.0,1.0));
    }   
    vec3 unitDirection = r.direction / length(r.direction);
    float t = 0.9*(unitDirection.y + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
}

void main(){

    //vec3 diffuseColor;
    //diffuseColor = texture(u_DiffuseMap, v_texCoord).rgb;

    // Creating ray object 
    ray r;
    r.origin = vec3(0.0, 0.0, 0.0);
    r.direction=FragPos;

    Sphere s1;
    s1.centerX = sphereCenter.x;
    s1.centerY = sphereCenter.y;
    s1.centerZ = sphereCenter.z; 
    s1.center = sphereCenter;

    int max_depth = 10;

    // Compute ambient light
    //vec3 ambient = light.ambient * material.ambient;

    // Compute diffuse light
    //vec3 norm = normalize(myNormal);
    //vec3 lightDir = normalize(light.position - FragPos);
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    //vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * (spec * material.specular);
    

    //vec3 result = diffuse + ambient + specular + rayColor(FragPos, sphereCenter, sphereRadius);
    //FragColor = vec4(rayColor(r, sphereCenter, sphereRadius), 1.0);
    FragColor = vec4(rayColor(r, s1, max_depth), 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}