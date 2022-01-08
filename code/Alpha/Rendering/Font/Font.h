/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>

#include "Rendering/Texture/Texture.h"
#include "Core/Resource/Resource.h"

#include "Core/Resource/ResourceMgr.h"

#include <DirectXMath.h>

class FontChar
{
public:

    // the unicode id
    int m_id;

    // these need to be converted to texture coordinates 
    // (where 0.0 is 0 and 1.0 is textureWidth of the font)
    float m_u; // u texture coordinate
    float m_v; // v texture coordinate
    float m_twidth; // width of character on texture
    float m_theight; // height of character on texture

    uint32_t m_width; // width of character in pixels
    uint32_t m_height; // height of character in pixels

    //all those values are in pixels.
    int32_t m_xoffset; // offset from current cursor pos to left side of character
    int32_t m_yoffset; // offset from top of line to top of character
    int32_t m_xadvance; // how far to move to right for next character
};

class FontKerning
{
public:
    int m_firstid; // the first character
    int m_secondid; // the second character
    int m_amount; // the amount to add/subtract to second characters x
};

class Font : public Resource
{
public:
    Font(const std::string& name);
    ~Font();

    void Init(const std::string& fntName);
    void Release() override;

    int GetKerning(char first, char second) const;
    FontChar* GetChar(char c) const;

    // Compute the rectangle in pixels containing the text
    void ComputeRect(const std::string& text, DirectX::XMUINT2& rect) const;

    std::string m_fontImage;
    int m_size; // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
    uint32_t m_lineHeight; // how far to move down to next line, will be normalized
    float m_baseHeight; // height of all characters, will be normalized
    int m_textureWidth; // width of the font texture
    int m_textureHeight; // height of the font texture
    int m_numCharacters; // number of characters in the font
    FontChar* m_CharList; // list of characters
    int m_numKernings; // the number of kernings
    FontKerning* m_KerningsList; // list to hold kerning values
    
    TextureId m_texture;

    // these are how much the character is padded in the texture. We
    // add padding to give sampling a little space so it does not accidentally
    // padd the surrounding characters. We will need to subtract these paddings
    // from the actual spacing between characters to remove the gaps you would otherwise see
    float m_leftpadding;
    float m_toppadding;
    float m_rightpadding;
    float m_bottompadding;

private:
    void LoadFntFile(const std::string& fntName);
};
