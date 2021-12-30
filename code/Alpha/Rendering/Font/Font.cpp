/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Rendering/Font/Font.h"

#include "Rendering/RenderModule.h"
#include "Helper.h"

#include <fstream>

Font::Font(const std::string& name)
    : Resource(name)
    , m_CharList(nullptr)
    , m_KerningsList(nullptr)
    , m_pSRVHeap(nullptr)
{}

Font::~Font()
{}

void Font::Init(const std::string& fntName, int windowWidth, int windowHeight)
{
    LoadFntFile(fntName, windowWidth, windowHeight);

    std::string textureFilename = "C:\\workspace\\Alpha\\data\\fonts\\" + m_fontImage;
    
    Texture* pTexture = g_pTextureMgr->CreateResource(m_texture, m_fontImage);
    pTexture->Init(textureFilename);

    //Create the SRV heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        HRESULT res = g_pRenderModule->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_pSRVHeap));
        ThrowIfFailed(res);
    }

    //Create the srv descriptor
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = pTexture->GetResourceDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;
        g_pRenderModule->GetDevice()->CreateShaderResourceView(pTexture->GetResource(), &srvDesc, m_pSRVHeap->GetCPUDescriptorHandleForHeapStart());
    }
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

float Font::GetKerning(char first, char second)
{
    for (int i = 0; i < m_numKernings; ++i)
    {
        if ((char)m_KerningsList[i].m_firstid == first && (char)m_KerningsList[i].m_secondid == second)
            return m_KerningsList[i].m_amount;
    }
    return 0.0f;
}

FontChar* Font::GetChar(char c)
{
    for (int i = 0; i < m_numCharacters; ++i)
    {
        if (c == (char)m_CharList[i].m_id)
            return &m_CharList[i];
    }
    return nullptr;
}

void Font::LoadFntFile(const std::string& fntName, int windowWidth, int windowHeight)
{
    std::ifstream fs;
    fs.open(fntName);

    std::string tmp;
    size_t startpos;

    // extract font name
    fs >> tmp; //info
    
    //face="Segoe UI"
    //the first getline stops at the first quote, the second getline stops at the second quote.
    std::getline(std::getline(fs, tmp, '\"'), m_name, '\"');

    // get font size
    fs >> tmp; // size=73
    startpos = tmp.find("=") + 1;
    m_size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
    fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 

    // get padding
    fs >> tmp; // padding=5,5,5,5 
    startpos = tmp.find("=") + 1;
    tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5

    // get up padding
    startpos = tmp.find(",") + 1;
    m_toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get right padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(",") + 1;
    m_rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get down padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    startpos = tmp.find(",") + 1;
    m_bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;

    // get left padding
    tmp = tmp.substr(startpos, tmp.size() - startpos);
    m_leftpadding = std::stoi(tmp) / (float)windowWidth;

    fs >> tmp; // spacing=0,0

    fs >> tmp; // outline

    // get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp >> tmp; // common lineHeight=95
    startpos = tmp.find("=") + 1;
    m_lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
    fs >> tmp; // base=68
    startpos = tmp.find("=") + 1;
    m_baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

    // get texture width
    fs >> tmp; // scaleW=512
    startpos = tmp.find("=") + 1;
    m_textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get texture height
    fs >> tmp; // scaleH=512
    startpos = tmp.find("=") + 1;
    m_textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // get pages, packed, page id
    fs >> tmp >> tmp; // pages=1 packed=0

    fs >> tmp >> tmp >> tmp >> tmp; //alphaChnl=1 redChnl=0 greenChnl=0 blueChnl=0

    //page id = 0
    fs >> tmp >> tmp;

    // get texture filename
    std::string wtmp;
    fs >> wtmp; // file="Arial.png"
    startpos = wtmp.find("\"") + 1;
        m_fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);

    // get number of characters
    fs >> tmp >> tmp; // chars count=97
    startpos = tmp.find("=") + 1;
    m_numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the character list
    m_CharList = new FontChar[m_numCharacters];

    for (int c = 0; c < m_numCharacters; ++c)
    {
        // get unicode id
        fs >> tmp >> tmp; // char id=0
        startpos = tmp.find("=") + 1;
        m_CharList[c].m_id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get x
        fs >> tmp; // x=392
        startpos = tmp.find("=") + 1;
        m_CharList[c].m_u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_textureWidth;

        // get y
        fs >> tmp; // y=340
        startpos = tmp.find("=") + 1;
        m_CharList[c].m_v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)m_textureHeight;

        // get width
        fs >> tmp; // width=47
        startpos = tmp.find("=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        m_CharList[c].m_width = (float)std::stoi(tmp) / (float)windowWidth;
        m_CharList[c].m_twidth = (float)std::stoi(tmp) / (float)m_textureWidth;

        // get height
        fs >> tmp; // height=57
        startpos = tmp.find("=") + 1;
        tmp = tmp.substr(startpos, tmp.size() - startpos);
        m_CharList[c].m_height = (float)std::stoi(tmp) / (float)windowHeight;
        m_CharList[c].m_theight = (float)std::stoi(tmp) / (float)m_textureHeight;

        // get xoffset
        fs >> tmp; // xoffset=-6
        startpos = tmp.find("=") + 1;
        m_CharList[c].m_xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get yoffset
        fs >> tmp; // yoffset=16
        startpos = tmp.find("=") + 1;
        m_CharList[c].m_yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;

        // get xadvance
        fs >> tmp; // xadvance=65
        startpos = tmp.find("=") + 1;
        m_CharList[c].m_xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;

        // get page
        // get channel
        fs >> tmp >> tmp; // page=0    chnl=0
    }

    // get number of kernings
    fs >> tmp >> tmp; // kernings count=96
    startpos = tmp.find("=") + 1;
    m_numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

    // initialize the kernings list
    m_KerningsList = new FontKerning[m_numKernings];

    for (int k = 0; k < m_numKernings; ++k)
    {
        // get first character
        fs >> tmp >> tmp; // kerning first=87
        startpos = tmp.find("=") + 1;
        m_KerningsList[k].m_firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get second character
        fs >> tmp; // second=45
        startpos = tmp.find("=") + 1;
        m_KerningsList[k].m_secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));

        // get amount
        fs >> tmp; // amount=-1
        startpos = tmp.find("=") + 1;
        int t = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
        m_KerningsList[k].m_amount = (float)t / (float)windowWidth;
    }
}
