#ifndef ANITO_PBR_COMMON_SH
#define ANITO_PBR_COMMON_SH

#define ANITO_PI 3.14159265359

// Phase 5 - Step 2: GGX Normal Distribution Function (D term)
// Reference: Filament PBR, Real-Time Rendering 4th Ed. Ch. 9
float D_GGX(float NoH, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float denom = (NoH * NoH * (a2 - 1.0) + 1.0);
    return a2 / max(ANITO_PI * denom * denom, 0.001);
}

// Phase 5 - Step 3: Smith GGX Visibility (height-correlated)
float V_SmithGGXCorrelated(float NoV, float NoL, float roughness)
{
    float a2 = roughness * roughness;
    float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
    return 0.5 / max(GGXV + GGXL, 0.001);
}

// Phase 5 - Step 4: Schlick Fresnel approximation
vec3 F_Schlick(float VoH, vec3 f0)
{
    float f = pow(1.0 - VoH, 5.0);
    return f0 + (1.0 - f0) * f;
}

// Phase 5 - Step 5: Full Cook-Torrance GGX specular BRDF
// Fr = D * V * F (using height-correlated visibility term)
vec3 BRDF_Specular_GGX(vec3 N, vec3 V, vec3 L, vec3 f0, float roughness)
{
    vec3 H = normalize(V + L);

    float NoH = max(dot(N, H), 0.0);
    float NoV = max(dot(N, V), 0.0);
    float NoL = max(dot(N, L), 0.0);
    float VoH = max(dot(V, H), 0.0);

    float D = D_GGX(NoH, roughness);
    float Vis = V_SmithGGXCorrelated(NoV, NoL, roughness);
    vec3 F = F_Schlick(VoH, f0);

    return D * Vis * F;
}

#endif // ANITO_PBR_COMMON_SH
