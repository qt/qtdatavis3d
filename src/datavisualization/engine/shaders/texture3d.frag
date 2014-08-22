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
    highp vec3 rayDir = -(cameraPositionRelativeToModel - pos);
    vec3 rayStart = pos;
    // Flip Y and Z so QImage bits work directly for texture and first image is in the front
    rayDir.yz = -rayDir.yz;
    rayStart.yz = -rayStart.yz;

    // Calculate ray intersection endpoint
    vec3 rayStop;
    if (rayDir.x == 0.0) {
        rayStop.yz = rayStart.yz;
        rayStop.x = -rayStart.x;
    } else if (rayDir.y == 0.0) {
        rayStop.xz = rayStart.xz;
        rayStop.y = -rayStart.y;
    } else if (rayDir.z == 0.0) {
        rayStop.xy = rayStart.xy;
        rayStop.z = -rayStart.z;
    } else {
        highp vec3 boxBounds = vec3(1.0, 1.0, 1.0);
        highp vec3 invRayDir = 1.0 / rayDir;
        if (rayDir.x < 0)
            boxBounds.x = -1.0;
        if (rayDir.y < 0)
            boxBounds.y = -1.0;
        if (rayDir.z < 0)
            boxBounds.z = -1.0;
        highp vec3 t = (boxBounds - rayStart) * invRayDir;
        highp float minT = min(t.x, t.y);
        minT = min(minT, t.z);
        rayStop = rayStart + minT * rayDir;
    }

    highp vec4 destColor = vec4(0, 0, 0, 0);
    highp float totalAlpha = 0.0;

    if (rayStart != rayStop) {
        // Convert intersections to texture coords
        rayStart = 0.5 * (rayStart + 1.0);
        rayStop = 0.5 * (rayStop + 1.0);

        highp vec3 ray = rayStop - rayStart;
        highp float fullDist = abs(length(ray));
        highp float rayX = abs(ray.x) * textureDimensions.x;
        highp float rayY = abs(ray.y) * textureDimensions.y;
        highp float rayZ = abs(ray.z) * textureDimensions.z;
        highp float maxRayDim = max(rayX, rayY);
        maxRayDim = max(maxRayDim, rayZ);
        int maxCount = int(floor(maxRayDim));

        highp vec3 step = ray / maxRayDim;
        highp float stepSize = fullDist / maxRayDim;

        rayStart += (step * 0.001);
        highp vec3 curPos = rayStart;

        // Adjust alpha multiplier according to the step size to get uniform alpha effect
        // regardless of the ray angle.
        highp float alphaMultiplier = stepSize / (1.0 / sampleCount);

        highp vec4 curColor = vec4(0, 0, 0, 0);
        highp vec3 curRgb = vec3(0, 0, 0);
        highp float curAlpha = 0.0;

        // Raytrace into volume, need to sample pixels along the eye ray until we hit opacity 1
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
            if (i == maxCount || totalAlpha >= 1.0)
                break;
            curPos += step;
        }
    }

    destColor.a = totalAlpha;
    gl_FragColor = clamp(destColor, 0.0, 1.0);
}
