#pragma once

#include <GLES3/gl3.h>
#include <zlib.h>
#include <vector>

#include "floating_logo_data.h"

namespace RamboFloatingLogo {

// One texture per GL context. EnsureLoaded() is called only while the renderer's
// OpenGL context is current.
inline GLuint gTexture = 0;

inline bool EnsureLoaded() {
    if (gTexture != 0 && glIsTexture(gTexture) == GL_TRUE) {
        return true;
    }
    gTexture = 0;

    std::vector<unsigned char> pixels(static_cast<size_t>(kFloatingLogoRawSize));
    uLongf outputSize = static_cast<uLongf>(pixels.size());
    const int result = uncompress(
        pixels.data(), &outputSize,
        reinterpret_cast<const Bytef*>(kFloatingLogoCompressed),
        static_cast<uLong>(kFloatingLogoCompressedSize));
    if (result != Z_OK || outputSize != pixels.size()) {
        return false;
    }

    while (glGetError() != GL_NO_ERROR) {
        // Clear stale renderer errors before validating this texture upload.
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    if (texture == 0) {
        return false;
    }

    GLint previousTexture = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
    GLint previousUnpackAlignment = 4;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 kFloatingLogoWidth, kFloatingLogoHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previousTexture));

    if (glGetError() != GL_NO_ERROR) {
        glDeleteTextures(1, &texture);
        return false;
    }
    gTexture = texture;
    return true;
}

inline void ResetForNewContext() {
    // A lost EGL context invalidates its texture IDs; do not delete them using a
    // replacement context because the numeric ID might have been reused.
    gTexture = 0;
}

} // namespace RamboFloatingLogo
