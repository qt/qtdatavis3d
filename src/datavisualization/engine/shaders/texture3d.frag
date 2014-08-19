#version 120

varying highp vec3 pos;

uniform highp sampler3D textureSampler;
uniform highp vec3 cameraPositionRelativeToModel;
uniform highp vec4 colorIndex[256];
uniform highp int color8Bit;
uniform highp vec3 textureDimensions;
uniform highp int sampleCount; // This is the maximum sample count

const highp float alphaThreshold = 0.0001;

void main() {
    // Raytrace into volume, need to sample pixels along the eye ray until we hit opacity 1

    // Find out where ray intersects the object
    highp vec3 rayDir = -(cameraPositionRelativeToModel - pos);
    highp vec3 invRayDir = 1.0 / rayDir;
    highp vec3 minCorner = vec3(-1.0);
    highp vec3 maxCorner = vec3(1.0);
    highp vec3 t1 = invRayDir * (minCorner - pos);
    highp vec3 t2 = invRayDir * (maxCorner - pos);
    highp vec3 tmin = min(t1, t2);
    highp vec3 tmax = max(t1, t2);
    highp vec2 t = max(tmin.xx, tmin.yz);
    t = min(tmax.xx, tmax.yz);
    float tFar = min(t.x, t.y);
    highp vec3 rayStart = pos;
    // Flip Y and Z so QImage bits work directly for texture and first image is in the front
    rayStart.yz = -rayStart.yz;
    highp vec3 rayStop = pos + rayDir * tFar;
    rayStop.yz = -rayStop.yz;

    // Convert intersections to texture coords
    rayStart = 0.5 * (rayStart + 1.0);
    rayStop = 0.5 * (rayStop + 1.0);

    highp vec3 ray = rayStop - rayStart;
    highp float fullDist = length(ray);
    highp float rayX = abs(ray.x) * textureDimensions.x;
    highp float rayY = abs(ray.y) * textureDimensions.y;
    highp float rayZ = abs(ray.z) * textureDimensions.z;

    highp float maxRayDim = max(rayX, rayY);
    maxRayDim = max(maxRayDim, rayZ);

    highp vec3 step = ray / maxRayDim;
    highp float stepSize = abs(fullDist / maxRayDim);

    // Offset a fraction of a step so we are not exactly on a texel boundary.
    highp vec3 curPos = rayStart - (0.5 * step);

    // Adjust alpha multiplier according to the step size to get uniform alpha effect
    // regardless of the ray angle.
    highp float alphaMultiplier = stepSize / (1.0 / sampleCount);

    highp float totalDist = 0.0;
    highp float totalAlpha = 0.0;
    highp vec4 destColor = vec4(0, 0, 0, 0);
    highp vec4 curColor = vec4(0, 0, 0, 0);
    highp vec3 curRgb = vec3(0, 0, 0);
    highp float curAlpha = 0.0;

    for (int i = 0; i < sampleCount; i++) {
        curColor = texture3D(textureSampler, curPos);
        if (color8Bit != 0)
            curColor = colorIndex[int(curColor.r * 255.0)];

        if (curColor.a == 1.0)
            curAlpha = 1.0;
        else
            curAlpha = clamp(curColor.a * alphaMultiplier, 0.0, 1.0);
        if (curAlpha > alphaThreshold) {
            curRgb = curColor.rgb * curAlpha * (1.0 - totalAlpha);
            destColor.rgb += curRgb;
            totalAlpha += curAlpha;
        }
        curPos += step;
        totalDist += stepSize;
        if (totalDist > fullDist || totalAlpha >= 1.0) {
            break;
        }
    }

    destColor.a = totalAlpha;
    gl_FragColor = clamp(destColor, 0.0, 1.0);
}

