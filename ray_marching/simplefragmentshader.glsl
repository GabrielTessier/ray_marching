#version 330 core

// UTILS

#define PI 3.14159265358979323846
#define DIST_CONTACT 0.01
#define MAX_STEPS 1000
#define MAX_DIST 100.

#define rot(a)      mat2( cos(a), -sin(a), sin(a), cos(a) )

uniform vec2 iResolution;
uniform float iTime;
uniform vec3 iPosCamera;
uniform vec3 iDirCamera;
uniform float iCameraTheta;
uniform float iCameraPhi;

layout (location = 0) out vec4 fragColor;

// CODE

// 0. for no grid, else size of the mesh (float)
#define GRID 0.

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p-center) - radius;
}

float clamp_zero(float a) {
    if (a<0) return 0.;
    return a;
}

float sdPlane(vec3 p, vec3 center, vec3 normal) {
    return clamp_zero(abs(dot(p - center, normal))-0.1);
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float radius) {
    float t = clamp(dot(b-a, p-a)/dot(b-a, b-a), 0., 1.);
    vec3 c = a + t*(b-a);
    return length(p-c) - radius;
}

vec2 sdUnion(vec2 a, vec2 b) {
    if (a.x < b.x) return a;
    return b;
}

vec2 map(vec3 p) {
    vec2 sphere = vec2(sdSphere(p, vec3(5,1,1), .25), 1.);
    vec2 plane = vec2(sdPlane(p, vec3(0, 0, 0), vec3(0, 1, 0)), 2.);
    vec2 capsule = vec2(sdCapsule(mod(p, 10), vec3(5, 4.5, 5), vec3(5, 5.5, 5), 2), 3.);
    vec2 sphereMirror1 = vec2(sdSphere(mod(p, 3), vec3(1.5,1.5,1.5), .5), 4.);
    vec2 sphereMirror2 = vec2(sdSphere(p, vec3(5,1,1), .5), 4.);
    vec2 sphereMirror3 = vec2(sdSphere(p, vec3(4,1,3), .5), 4.);
    vec2 sphereMirror4 = vec2(sdSphere(p, vec3(2,1,2), .5), 4.);
    vec2 sphereMirror5 = vec2(sdSphere(p, vec3(5,1,-1), .5), 4.);


    vec2 sphere2 = vec2(sdSphere(p, vec3(9,1,9), .5), 1.);
    vec2 sphere3 = vec2(sdSphere(p, vec3(9,1,8), .5), 1.);

    vec2 planeMirror1 = vec2(sdPlane(p, vec3(10, 1, 10), vec3(-1, 0, 0)), 1.);
    //vec2 planeMirror2 = vec2(sdPlane(p, vec3(-10, 1, 10), vec3(-1, 0, 0)), 4.);

    //vec2 res = sphere;
    vec2 res = sphereMirror1;
    res = sdUnion(res, sphere);
    /*res = sdUnion(res, plane);
    res = sdUnion(res, capsule);
    res = sdUnion(res, sphereMirror1);
    res = sdUnion(res, sphereMirror2);
    res = sdUnion(res, sphereMirror3);
    res = sdUnion(res, sphereMirror4);
    res = sdUnion(res, sphereMirror5);
    res = sdUnion(res, sphere2);
    res = sdUnion(res, sphere3);
    res = sdUnion(res, planeMirror1);*/
    //res = sdUnion(res, planeMirror2);
    return res;
}

vec3 getNormal(vec3 p) {
    float d = map(p).x;
    vec2 e = vec2(.01, 0);
    vec3 n = d - vec3(map(p-e.xyy).x,
                      map(p-e.yxy).x,
                      map(p-e.yyx).x);
    return normalize(n);
}

vec2 RayMarching(vec3 ro, vec3 rd) {
    float dO = 0.;
    for (int i=0; i<MAX_STEPS; i++) {
        vec3 p = ro + rd*dO;
        vec2 hit = map(p); 
        float dS = hit.x;
        dO += dS;
        if (dS<DIST_CONTACT || dO>MAX_DIST) {
            return vec2(dO, hit.y);
        }
    }
    return vec2(dO, 0.);
}

float getLight(vec3 p) {
    vec3 lo = vec3(0., 4, -10.); // light origin
    lo.xz += 2.*vec2(cos(iTime), sin(iTime));
    
    vec3 n = getNormal(p); // surface normal at point p
    vec3 l = normalize(lo-p); // light vector
    
    float dif = clamp(dot(n, l), 0.2, 1.); // dot product can be negative
    vec2 ray = RayMarching(p+n*DIST_CONTACT*2., l);
    if (ray.x < length(lo-p)) return 0.2;
    
    //if (length(lo - p) < 1.) return 1.;
    
    return dif;
}

vec3 getColor(float val) {
    if (val == 0.) return vec3(1.);
    if (val == 1.) return vec3(1., 0., 0.);
    if (val == 2.) return vec3(0., 1., 0.);
    if (val == 3.) return vec3(0., 0., 1.);
    if (val == 4.) return vec3(0.);
    return vec3(0.);
}

vec3 render(vec2 uv, vec3 pos, vec3 dir, int nb_reflexion) {
    vec3 col = vec3(1.);

    while (nb_reflexion > 0.) {
        vec2 ray = RayMarching(pos, dir);
        float dS = ray.x;
        col = getColor(ray.y);
        if (dS < MAX_DIST) {
            vec3 p = pos + dir*dS; // hit point
            if (ray.y == 4.) {
                vec3 normal = getNormal(p);
                vec3 projeter = normal*dot(normal, dir) - dir;
                vec3 new_dir = -(dir + projeter*2.);
                // col = render(uv, p+new_dir*DIST_CONTACT*2., new_dir, nb_reflexion-1);
                pos = p+new_dir*DIST_CONTACT*3.;
                dir = new_dir;
                nb_reflexion = nb_reflexion-1;
                //return vec3(1.);
                continue;
            } else {
                float light = getLight(p);
                col *= light;
            }
        } else { // sky
            col = vec3(53, 81, 92)/100. - uv.y/4.;
        }
        break;
    }
    return col;
}

void main()
{
    vec2 uv = (2.0 * gl_FragCoord.xy - iResolution.xy) / iResolution.y;

    vec3 pos_camera = iPosCamera;
    vec3 dir_camera = iDirCamera;

    // Calculation of the ray from the camera corresponding to uv
    vec3 droite = vec3(-sin(iCameraTheta), 0., cos(iCameraTheta));
    vec3 devant = dir_camera;
    vec3 haut = cross(droite, devant);
    vec3 rd = normalize(droite*uv.x + haut*uv.y + devant*2.);

    // color of pixel
    vec3 col = render(uv, pos_camera, rd, 5);

    fragColor = vec4(col, 1.0);
}
