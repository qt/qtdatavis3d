#version 120

varying highp vec3 pos;

uniform highp sampler3D textureSampler;
uniform highp vec3 cameraPositionRelativeToModel;
uniform highp vec3 volumeSliceIndices;
uniform highp vec4 colorIndex[256];
uniform highp int color8Bit;

const highp vec3 xPlaneNormal = vec3(1.0, 0, 0);
const highp vec3 yPlaneNormal = vec3(0, 1.0, 0);
const highp vec3 zPlaneNormal = vec3(0, 0, 1.0);

const highp float alphaThreshold = 0.0001;

void main() {
    // Raytrace into volume, need to sample pixels along the eye ray until we hit opacity 1

    // Find out where ray intersects the slice planes
    highp vec3 rayDir = -(cameraPositionRelativeToModel - pos);
    highp vec3 rayStart = pos;
    // Flip Y and Z so QImage bits work directly for texture and first image is in the front
    rayStart.yz = -rayStart.yz;
    rayDir.yz = -rayDir.yz;
    highp vec3 invRayDir = 1.0 / rayDir;
    highp vec3 minCorner = vec3(-1.0);
    highp vec3 maxCorner = vec3(1.0);
    highp vec3 t1 = invRayDir * (minCorner - rayStart);
    highp vec3 t2 = invRayDir * (maxCorner - rayStart);
    highp vec3 tmin = min(t1, t2);
    highp vec3 tmax = max(t1, t2);
    highp vec2 t = max(tmin.xx, tmin.yz);
    t = min(tmax.xx, tmax.yz);
    float tFar = min(t.x, t.y);

    highp vec3 xPoint = vec3(volumeSliceIndices.x, 0, 0);
    highp vec3 yPoint = vec3(0, volumeSliceIndices.y, 0);
    highp vec3 zPoint = vec3(0, 0, volumeSliceIndices.z);
    highp float firstD = tFar + 1.0;
    highp float secondD = firstD;
    highp float thirdD = firstD;
    if (volumeSliceIndices.x >= -1.0) {
        highp float dx = dot(xPoint - rayStart, xPlaneNormal) / dot(rayDir, xPlaneNormal);
        if (dx >= 0.0 && dx <= tFar)
            firstD = min(dx, firstD);
    }
    if (volumeSliceIndices.y >= -1.0) {
        highp float dy = dot(yPoint - rayStart, yPlaneNormal) / dot(rayDir, yPlaneNormal);
        if (dy >= 0.0 && dy <= tFar) {
            if (dy < firstD) {
                secondD = firstD;
                firstD = dy;
            } else {
                secondD = dy;
            }
        }
    }
    if (volumeSliceIndices.z >= -1.0) {
        highp float dz = dot(zPoint - rayStart, zPlaneNormal) / dot(rayDir, zPlaneNormal);
        if (dz >= 0.0) {
            if (dz < firstD && dz <= tFar) {
                thirdD = secondD;
                secondD = firstD;
                firstD = dz;
            } else if (dz < secondD){
                thirdD = secondD;
                secondD = dz;
            } else {
                thirdD = dz;
            }
        }
    }

    highp vec4 destColor = vec4(0.0, 0.0, 0.0, 0.0);
    highp float totalAlpha = 0.0;
    highp vec3 curRgb = vec3(0, 0, 0);

    // Convert intersection to texture coords

    if (firstD <= tFar) {
        highp vec3 firstTex = rayStart + rayDir * firstD;
        firstTex = 0.5 * (firstTex + 1.0);
        highp vec4 firstColor = texture3D(textureSampler, firstTex);
        if (color8Bit != 0)
            firstColor = colorIndex[int(firstColor.r * 255.0)];

        if (firstColor.a > alphaThreshold) {
            destColor.rgb = firstColor.rgb * firstColor.a;
            totalAlpha = firstColor.a;
        }
        if (secondD <= tFar && totalAlpha < 1.0) {
            highp vec3 secondTex = rayStart + rayDir * secondD;
            secondTex = 0.5 * (secondTex + 1.0);
            highp vec4 secondColor = texture3D(textureSampler, secondTex);
            if (color8Bit != 0)
                secondColor = colorIndex[int(secondColor.r * 255.0)];
            if (secondColor.a > alphaThreshold) {
                curRgb = secondColor.rgb * secondColor.a * (1.0 - totalAlpha);
                destColor.rgb += curRgb;
                totalAlpha += secondColor.a;
            }
            if (thirdD <= tFar && totalAlpha < 1.0) {
                highp vec3 thirdTex = rayStart + rayDir * thirdD;
                thirdTex = 0.5 * (thirdTex + 1.0);
                highp vec4 thirdColor = texture3D(textureSampler, thirdTex);
                if (color8Bit != 0)
                    thirdColor = colorIndex[int(thirdColor.r * 255.0)];
                if (thirdColor.a > alphaThreshold) {
                    curRgb = thirdColor.rgb * thirdColor.a * (1.0 - totalAlpha);
                    destColor.rgb += curRgb;
                    totalAlpha += thirdColor.a;
                }
            }
        }
    }
    destColor.a = totalAlpha;
    gl_FragColor = clamp(destColor, 0.0, 1.0);
}

