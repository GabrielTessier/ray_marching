#version 330 core

// UTILS

#define PI 3.14159265358979323846
#define DIST_CONTACT 0.01
#define MAX_STEPS 1000
#define MAX_DIST 100.

#define rot(a)      mat2( cos(a), -sin(a), sin(a), cos(a) )

uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iTime;

layout (location = 0) out vec4 fragColor;

// CODE

// 0. for no grid, else size of the mesh (float)
#define GRID 0.

float sdSphere(vec3 p, vec3 center, float radius) {
    return length(p-center) - radius;
}

float sdPlane(vec3 p, vec3 center, vec3 normal) {
    return dot(p - center, normal);
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float radius) {
    float t = clamp(dot(b-a, p-a)/dot(b-a, b-a), 0., 1.);
    vec3 c = a + t*(b-a);
    return length(p-c) - radius;
}
    

float getDist(vec3 p) { 
    float ds = sdSphere(p, vec3(0,1,-5), .5);
    float dp = sdPlane(p, vec3(0, 0, 0), vec3(0, 1, 0));
    float dc = sdCapsule(p, vec3(-1, 1, -5), vec3(-1, 2, -5), .5);
    
    return min(dp, min(ds, dc));
}

vec3 getNormal(vec3 p) {
    float d = getDist(p);
    vec2 e = vec2(.01, 0);
    vec3 n = d - vec3(getDist(p-e.xyy),
                      getDist(p-e.yxy),
                      getDist(p-e.yyx));
    return normalize(n);
}

float RayMarching(vec3 ro, vec3 rd) {
    float dO = 0.;
    for (int i=0; i<MAX_STEPS; i++) {
        vec3 p = ro + rd*dO;
        float dS = getDist(p);
        dO += dS;
        if (dS<DIST_CONTACT || dO>MAX_DIST) break;
    }
    return dO;
}

float getLight(vec3 p) {
    vec3 lo = vec3(0., 1, -5.); // light origin
    lo.xz += 2.*vec2(cos(iTime), sin(iTime));
    
    vec3 n = getNormal(p); // surface normal at point p
    vec3 l = normalize(lo-p); // light vector
    
    float dif = clamp(dot(n, l), 0., 1.); // dot product can be negative
    float d = RayMarching(p+n*DIST_CONTACT*2., l);
    if (d < length(lo-p)) dif *= .1;
    
    //if (length(lo - p) < 1.) return 1.;
    
    return dif;
}

void main()
{
    vec2 uv = gl_FragCoord.xy/iResolution.xy; // normalized pixel coordinates (from 0 to 1)
    uv -= 0.5; // centers uv
    // uv.x *= iResolution.x/iResolution.y; // scales uv correctly
    
    //vec3 pos_camera = vec3(0, 5, -5.*iTime);
    vec3 pos_camera = vec3(0, 1, 1);
    vec3 dir_camera = vec3(0, 0, -1);
    float dist_viewport = 2.;
    float width_viewport = 2.;
    float height_viewport = width_viewport * iResolution.y/iResolution.x;
    
    // Move camera with mouse
    vec2 m = iMouse.xy/iResolution.xy - 0.5; 
    dir_camera += vec3(6.*m.x, m.y, 0);
    dir_camera = dir_camera/length(dir_camera);

    // Calculation of the ray from the camera corresponding to uv                   
    vec2  pdu = vec2(dir_camera.x, dir_camera.z) * rot(PI/2.);
    
    vec3 du = vec3(pdu.x, 0, pdu.y);
    vec3 dv;
    if (dir_camera.y != 0.) {
        dv = vec3(-dir_camera.x, 
                  (dir_camera.x*dir_camera.x + dir_camera.z*dir_camera.z) / abs(dir_camera.y), 
                  -dir_camera.z);
    } else
        dv = vec3(0, 1, 0);
        
    du = normalize(du) * width_viewport;
    dv = normalize(dv) * height_viewport;
    vec3 rd = normalize(dir_camera*dist_viewport + uv.x*du + uv.y*dv);    
    
    // color of pixel
    vec3 col;
    float dS = RayMarching(pos_camera, rd);
    if (dS < MAX_DIST) {
        vec3 p = pos_camera + rd*dS; // hit point
        col = vec3(getLight(p));
    } else { // sky
        col = vec3(53, 81, 92)/100. - uv.y/4.;
        //col = texture(iChannel2, uv+dir_camera.xy*vec2(1, 1.5)).xyz;
    }
    
    fragColor = vec4(col, 1.0);
}