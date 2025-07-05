/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Font/Font.h"

#include "Rendering/Texture/TextureMgr.h"
#include "Rendering/Texture/Texture.h"
#include "Core/Helper.h"

#include <fstream>
#include <sstream>

namespace Rendering
{
    Font::Font()
        : m_name()
        , m_CharList(nullptr)
        , m_KerningsList(nullptr)
        , m_baseHeight()
        , m_bottompadding()
    {}

    Font::~Font()
    {}

    void Font::Init(const char* pFntFileData, uint32_t fntFileDataSize, const char* pFntTextureData, uint32_t fntTextureDataSize)
    {
        LoadFntFile(pFntFileData, fntFileDataSize);

        Texture* pTexture = nullptr;
        Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_texture);
        pTexture->Init(pFntTextureData, fntTextureDataSize);
    }

    void Font::Init(const std::string& fontPath, const std::string& fontName)
    {
        std::string fontFilename = fontPath + "\\" + fontName + ".fnt";
        LoadFntFile(fontFilename);

        std::string textureFilename = fontPath + "\\" + m_fontImage;

        Texture* pTexture = nullptr;
        Rendering::TextureMgr::Get().CreateTexture(&pTexture, m_texture);
        pTexture->Init(textureFilename);
    }

    void Font::Release()
    {
        if (m_CharList)
        {
            delete m_CharList;
            m_CharList = nullptr;
        }

        if (m_KerningsList)
        {
            delete m_KerningsList;
            m_KerningsList = nullptr;
        }
    }

    int Font::GetKerning(char first, char second) const
    {
        for (int i = 0; i < m_numKernings; ++i)
        {
            if ((char)m_KerningsList[i].m_firstid == first && (char)m_KerningsList[i].m_secondid == second)
                return m_KerningsList[i].m_amount;
        }
        return 0;
    }

    FontChar* Font::GetChar(char c) const
    {
        // all ascii characters below 32 don't have a visual representation
        if (c < 32)
            c = ' ';

        for (int i = 0; i < m_numCharacters; ++i)
        {
            if (c == (char)m_CharList[i].m_id)
                return &m_CharList[i];
        }
        return nullptr;
    }

    void Font::ComputeRect(const std::string& text, DirectX::XMUINT2& rect) const
    {
        rect.x = 0;
        rect.y = m_lineHeight;
        char lastChar = -1; // no last character to start with
        for (int i = 0; i < text.size(); ++i)
        {
            char c = text[i];
            const FontChar* fc = GetChar(c);
            // character not in font char set
            if (fc == nullptr)
            {
                OutputDebugString("WARNING : Character not found in the font");
                break;
            }
            // end of string
            if (c == '\0')
                break;
            int kerning = 0;
            if (i > 0)
                kerning = GetKerning(lastChar, c);
            rect.x += fc->m_xadvance;

            lastChar = c;
        }
    }

    void Font::Internal_LoadFntFile(std::istream& stream)
    {
        std::string tmp;
        size_t startpos;

        // extract font name
        stream >> tmp; //info

        //face="Segoe UI"
        //the first getline stops at the first quote, the second getline stops at the second quote.
        std::getline(std::getline(stream, tmp, '\"'), m_name, '\"');

        // get font size
        stream >> tmp; // size=73
        startpos = tmp.find("=") + 1;
        m_size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
        stream >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

        // get padding
        stream >> tmp; // padding=5,5,5,5 
        startpos = tmp.find("=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

        // get up padding
        startpos = tmp.find(",") + 1;
        m_toppadding = (float)std::stoi(tmp.substr(0, startpos));// / (float)windowWidth;

        // get right padding
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        startpos = tmp.find(",") + 1;
        m_rightpadding = (float)std::stoi(tmp.substr(0, startpos));// / (float)windowWidth;

        // get down padding
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        startpos = tmp.find(",") + 1;
        m_bottompadding = (float)std::stoi(tmp.substr(0, startpos));// / (float)windowWidth;

        // get left padding
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        m_leftpadding = (float)std::stoi(tmp);// / (float)windowWidth;

        stream >> tmp; // spacing=0,0

        stream >> tmp; // outline

        // get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
        stream >> tmp >> tmp; // common lineHeight=95
        startpos = tmp.find("=") + 1;
        m_lineHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
        stream >> tmp; // base=68
        startpos = tmp.find("=") + 1;
        m_baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));// / (float)windowHeight;

        // get texture width
        stream >> tmp; // scaleW=512
        startpos = tmp.find("=") + 1;
        m_textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get texture height
        stream >> tmp; // scaleH=512
        startpos = tmp.find("=") + 1;
        m_textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get pages, packed, page id
        stream >> tmp >> tmp; // pages=1 packed=0

        stream >> tmp >> tmp >> tmp >> tmp; //alphaChnl=1 redChnl=0 greenChnl=0 blueChnl=0

        //page id = 0
        stream >> tmp >> tmp;

        // get texture filename
        std::string wtmp;
        stream >> wtmp; // file="Arial.png"
        startpos = wtmp.find("\"") + 1;
        m_fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

        // get number of characters
        stream >> tmp >> tmp; // chars count=97
        startpos = tmp.find("=") + 1;
        m_numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // initialize the character list
        m_CharList = new FontChar[m_numCharacters];

        for (int c = 0; c < m_numCharacters; ++c)
        {
            // get unicode id
            stream >> tmp >> tmp; // char id=0
            startpos = tmp.find("=") + 1;
            m_CharList[c].m_id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

            // get x
            stream >> tmp; // x=392
            startpos = tmp.find("=") + 1;
            m_CharList[c].m_u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_textureWidth;

            // get y
            stream >> tmp; // y=340
            startpos = tmp.find("=") + 1;
            m_CharList[c].m_v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_textureHeight;

            // get width
            stream >> tmp; // width=47
            startpos = tmp.find("=") + 1;
            tmp = tmp.substr(startpos, tmp.size() - startpos);
            m_CharList[c].m_width = std::stoi(tmp);
            m_CharList[c].m_twidth = (float)std::stoi(tmp) / (float)m_textureWidth;

            // get height
            stream >> tmp; // height=57
            startpos = tmp.find("=") + 1;
            tmp = tmp.substr(startpos, tmp.size() - startpos);
            m_CharList[c].m_height = std::stoi(tmp);
            m_CharList[c].m_theight = (float)std::stoi(tmp) / (float)m_textureHeight;

            // get xoffset
            stream >> tmp; // xoffset=-6
            startpos = tmp.find("=") + 1;
            m_CharList[c].m_xoffset = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

            // get yoffset
            stream >> tmp; // yoffset=16
            startpos = tmp.find("=") + 1;
            m_CharList[c].m_yoffset = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

            // get xadvance
            stream >> tmp; // xadvance=65
            startpos = tmp.find("=") + 1;
            m_CharList[c].m_xadvance = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

            // get page
            // get channel
            stream >> tmp >> tmp; // page=0    chnl=0
        }

        // get number of kernings
        stream >> tmp >> tmp; // kernings count=96
        startpos = tmp.find("=") + 1;
        m_numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // initialize the kernings list
        m_KerningsList = new FontKerning[m_numKernings];

        for (int k = 0; k < m_numKernings; ++k)
        {
            // get first character
            stream >> tmp >> tmp; // kerning first=87
            startpos = tmp.find("=") + 1;
            m_KerningsList[k].m_firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

            // get second character
            stream >> tmp; // second=45
            startpos = tmp.find("=") + 1;
            m_KerningsList[k].m_secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

            // get amount
            stream >> tmp; // amount=-1
            startpos = tmp.find("=") + 1;
            int t = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
            m_KerningsList[k].m_amount = t;
        }
    }

    void Font::LoadFntFile(const std::string& fntName)
    {
        std::ifstream fs;
        fs.open(fntName);

        Internal_LoadFntFile(fs);

        fs.close();
    }

    void Font::LoadFntFile(const char* pBuffer, uint32_t size)
    {
        std::istringstream stream(std::string(pBuffer, size));
        Internal_LoadFntFile(stream);
    }
}
