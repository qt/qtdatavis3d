#version 120

varying highp vec3 pos;

uniform highp sampler3D textureSampler;
uniform highp vec3 cameraPositionRelativeToModel;
uniform highp vec3 volumeSliceIndices;
uniform highp vec4 colorIndex[256];
uniform highp int color8Bit;
uniform highp float alphaMultiplier;
uniform highp int preserveOpacity;

const highp vec3 xPlaneNormal = vec3(1.0, 0, 0);
const highp vec3 yPlaneNormal = vec3(0, 1.0, 0);
const highp vec3 zPlaneNormal = vec3(0, 0, 1.0);

void main() {
    // Find out where ray intersects the slice planes
    highp vec3 rayDir = -(cameraPositionRelativeToModel - pos);
    rayDir = normalize(rayDir);
    highp vec3 rayStart = pos;
    // Flip Y and Z so QImage bits work directly for texture and first image is in the front
    rayStart.yz = -rayStart.yz;
    rayDir.yz = -rayDir.yz;
    highp float minT = 2.0f;
    if (rayDir.x != 0.0 && rayDir.y != 0.0 && rayDir.z != 0.0) {
        highp vec3 boxBounds = vec3(1.0, 1.0, 1.0);
        highp vec3 invRayDir = 1.0 / rayDir;
        if (rayDir.x < 0)
            boxBounds.x = -1.0;
        if (rayDir.y < 0)
            boxBounds.y = -1.0;
        if (rayDir.z < 0)
            boxBounds.z = -1.0;
        highp vec3 t = (boxBounds - rayStart) * invRayDir;
        minT = min(t.x, min(t.y, t.z));
    }

    highp vec3 xPoint = vec3(volumeSliceIndices.x, 0, 0);
    highp vec3 yPoint = vec3(0, volumeSliceIndices.y, 0);
    highp vec3 zPoint = vec3(0, 0, volumeSliceIndices.z);
    highp float firstD = minT + 1.0;
    highp float secondD = firstD;
    highp float thirdD = firstD;
    if (volumeSliceIndices.x >= -1.0) {
        highp float dx = dot(xPoint - rayStart, xPlaneNormal) / dot(rayDir, xPlaneNormal);
        if (dx >= 0.0 && dx <= minT)
            firstD = min(dx, firstD);
    }
    if (volumeSliceIndices.y >= -1.0) {
        highp float dy = dot(yPoint - rayStart, yPlaneNormal) / dot(rayDir, yPlaneNormal);
        if (dy >= 0.0 && dy <= minT) {
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
            if (dz < firstD && dz <= minT) {
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
    highp vec4 curColor = vec4(0.0, 0.0, 0.0, 0.0);
    highp float totalAlpha = 0.0;
    highp vec3 curRgb = vec3(0, 0, 0);
    highp float curAlpha = 0.0;

    // Convert intersection to texture coords

    if (firstD <= minT) {
        highp vec3 firstTex = rayStart + rayDir * firstD;
        firstTex = 0.5 * (firstTex + 1.0);
        curColor = texture3D(textureSampler, firstTex);
        if (color8Bit != 0)
            curColor = colorIndex[int(curColor.r * 255.0)];

        if (curColor.a > 0.0) {
            curAlpha = curColor.a;
            if (curColor.a == 1.0 && preserveOpacity != 0)
                curAlpha = 1.0;
            else
                curAlpha = clamp(curColor.a * alphaMultiplier, 0.0, 1.0);
            destColor.rgb = curColor.rgb * curAlpha;
            totalAlpha = curAlpha;
        }
        if (secondD <= minT && totalAlpha < 1.0) {
            highp vec3 secondTex = rayStart + rayDir * secondD;
            secondTex = 0.5 * (secondTex + 1.0);
            curColor = texture3D(textureSampler, secondTex);
            if (color8Bit != 0)
                curColor = colorIndex[int(curColor.r * 255.0)];
            if (curColor.a > 0.0) {
                if (curColor.a == 1.0 && preserveOpacity != 0)
                    curAlpha = 1.0;
                else
                    curAlpha = clamp(curColor.a * alphaMultiplier, 0.0, 1.0);
                curRgb = curColor.rgb * curAlpha * (1.0 - totalAlpha);
                destColor.rgb += curRgb;
                totalAlpha += curAlpha;
            }
            if (thirdD <= minT && totalAlpha < 1.0) {
                highp vec3 thirdTex = rayStart + rayDir * thirdD;
                thirdTex = 0.5 * (thirdTex + 1.0);
                curColor = texture3D(textureSampler, thirdTex);
                if (curColor.a > 0.0) {
                    if (color8Bit != 0)
                        curColor = colorIndex[int(curColor.r * 255.0)];
                    if (curColor.a == 1.0 && preserveOpacity != 0)
                        curAlpha = 1.0;
                    else
                        curAlpha = clamp(curColor.a * alphaMultiplier, 0.0, 1.0);
                    curRgb = curColor.rgb * curAlpha * (1.0 - totalAlpha);
                    destColor.rgb += curRgb;
                    totalAlpha += curAlpha;
                }
            }
        }
    }

    // Brighten up the final color if there is some transparency left
    if (totalAlpha > 0.0 && totalAlpha < 1.0)
        destColor *= 1.0 / totalAlpha;

    destColor.a = totalAlpha;
    gl_FragColor = clamp(destColor, 0.0, 1.0);
}

