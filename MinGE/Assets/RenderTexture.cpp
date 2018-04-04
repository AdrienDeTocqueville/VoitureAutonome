#include "Assets/RenderTexture.h"

#include "Utility/Error.h"

RenderTexture::RenderTexture(unsigned _width, unsigned _height, unsigned _depth):
    width(_width), height(_height),
    colorBuffer(nullptr), depthBuffer(nullptr)
{
    colorBuffer = new Texture(width, height);

    if (_depth == 16)
        depthBuffer = new RenderBuffer(width, height, GL_DEPTH_COMPONENT16);
    else if (_depth == 24)
        depthBuffer = new RenderBuffer(width, height, GL_DEPTH_COMPONENT24);
    else if (_depth != 0)
        Error::add(ErrorType::USER_ERROR, "RenderTexture::RenderTexture() -> Invalid render buffer depth. Must be 0, 16 or 24. Is: " + toString(_depth));
}

RenderTexture::~RenderTexture()
{
    colorBuffer->destroy();
    depthBuffer->destroy();
}

Texture* RenderTexture::getColorBuffer() const
{
    return colorBuffer;
}

RenderBuffer* RenderTexture::getDepthBuffer() const
{
    return depthBuffer;
}

ivec2 RenderTexture::getSize() const
{
    return vec2(width, height);
}
