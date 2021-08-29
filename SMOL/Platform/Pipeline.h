#pragma once

#include "pc.h"

#include "Misc/Property.h"
#include "Misc/Utils.h"
#include <stack>

enum GPUWriteMask {
    GPUWriteMask_Red   = 1, 
    GPUWriteMask_Green = 2, 
    GPUWriteMask_Blue  = 4, 
    GPUWriteMask_Alpha = 8, 
    GPUWriteMask_All   = (GPUWriteMask_Red | GPUWriteMask_Green | GPUWriteMask_Blue | GPUWriteMask_Alpha),

    GPUWriteMask_Count = 9
};

enum class GPUInputClassification {
    PerVertexData, 
    PerInstanceData,

    Count
};

enum class GPUTexAddressMode {
    Wrap       = 1, 
    Mirror     = 2, 
    Clamp      = 3, 
    Border     = 4, 
    MirrorOnce = 5,

    Count
};

enum class GPUFillMode {
    Wire = 2, Solid = 3,

    Count
};

enum class GPUCullMode {
    None = 1, Front = 2, Back = 3,

    Count
};

enum class GPUBlend {
    Zero           = 1,   // 0
    One            = 2,   // 1
    SrcColor       = 3,   // 2
    InvSrcColor    = 4,   // 3

    SrcAlpha       = 5,   // 6
    InvSrcAlpha    = 6,   // 7
    DestAlpha      = 7,   // 8
    InvDestAlpha   = 8,   // 9

    DestColor      = 9,   // 4
    InvDestColor   = 10,  // 5

    SrcAlphaSat    = 11,  // 14

    BlendFactor    = 14,  // 10
    InvBlendFactor = 15,  // 11
    Src1Color      = 16,  // 15
    InvSrc1Color   = 17,  // 16
    Src1Alpha      = 18,  // 17
    InvSrc1Alpha   = 19,  // 18

    Count
};

enum class GPUBlendOp {
    Add    = 1,  // 0
    Sub    = 2,  // 1
    RevSub = 3,  // 2
    Min    = 4,  // 3
    Max    = 5,  // 4

    Count
};

enum class GPUCompareFunc {
    Never    = 1, 
    Less     = 2, 
    Equal    = 3, 
    LEQ      = 4, 
    Greater  = 5, 
    NotEqual = 6, 
    GEQ      = 7, 
    Always   = 8,

    Count
};

enum class GPUStencilOp {
    Keep    = 1, 
    Zero    = 2, 
    Replace = 3, 
    Inc_sat = 4, 
    Dec_sat = 5, 
    Invert  = 6, 
    Inc     = 7, 
    Dec     = 8,

    Count
};

enum class TextureQuality {
    LowNoAlpha,  // PSP: 5650
    LowAlpha,    // PSP: 5551
    Medium,      // PSP: 4444
    High,        // PSP: 8888

    DXT1, 
    DXT3, 
    DXT5, 

    Depth16,
    Depth24x8,
    Depth32,

    Count
};

enum class GPUTopology {
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    QuadList,

    Count
};

template<typename T>
class Stack final: std::stack<T> {
public:
    T& operator=(T& rhs) { this->push(rhs); return rhs; }
    T& operator->() { return this->top(); }
    const T& operator->() const { return this->top(); }
    T& back() { return this->c.front(); }
    const T& back() const { return this->c.front(); }
};

enum VertexFormat: u32 {
    VFTexture8  = 0x001, 
    VFTexture16 = 0x002, 
    VFTexture32 = 0x004, 

    VFColorLowNoAlpha = 0x008, 
    VFColorLowAlpha   = 0x010, 
    VFColorMedium     = 0x020, 
    VFColorHigh       = 0x040, 

    VFNormal8  = 0x080, 
    VFNormal16 = 0x100, 
    VFNormal32 = 0x200, 
    
    VFVertex8  = 0x0400, 
    VFVertex16 = 0x0800, 
    VFVertex32 = 0x1000, 
    
    VFWeight8  = 0x2000, 
    VFWeight16 = 0x4000, 
    VFWeight32 = 0x8000, 

    /*VFIndex8  = 0x10000, 
    VFIndex16 = 0x20000, */

    //VFTransform2D = 0x40000, 
    //VFTransform3D = 0x80000, 
};

template<u32 N, u32 N2>
u32 Convert(u32 value) {
    f32 v = ToF32<u32>(value);
    v /= ToF32<u32>((1 << N) - 1);

    return ToU32(v * ToF32<u32>((1 << N2) - 1));
}

struct ColorLowNoAlpha
{
    union
    {
        u16 color;

        struct
        {
            u16 r : 5;
            u16 g : 6;
            u16 b : 5;
        };
    };

    u32 GetRed()   const { return Convert<5, 8>(r); }
    u32 GetGreen() const { return Convert<6, 8>(g); }
    u32 GetBlue()  const { return Convert<5, 8>(b); }

    void SetRed(u32 v)   { r = Convert<8, 5>(v); }
    void SetGreen(u32 v) { g = Convert<8, 6>(v); }
    void SetBlue(u32 v)  { b = Convert<8, 5>(v); }
    void SetColor(u32 v) { SetRed(v & 0xFF); SetGreen((v >> 8) & 0xFF); SetBlue((v >> 16) & 0xFF); };
};

struct ColorLowAlpha
{
    union
    {
        u16 color;

        struct
        {
            u16 r : 5;
            u16 g : 5;
            u16 b : 5;
            u16 a : 1;
        };
    };

    u32 GetRed()   const { return Convert<5, 8>(r); }
    u32 GetGreen() const { return Convert<6, 8>(g); }
    u32 GetBlue()  const { return Convert<5, 8>(b); }
    u32 GetAlpha() const { return Convert<1, 8>(a); }

    void SetRed(u32 v)   { r = Convert<8, 5>(v); }
    void SetGreen(u32 v) { g = Convert<8, 6>(v); }
    void SetBlue(u32 v)  { b = Convert<8, 5>(v); }
    void SetAlpha(u32 v) { b = Convert<8, 1>(v); }
    void SetColor(u32 v) { SetRed(v & 0xFF); SetGreen((v >> 8) & 0xFF); SetBlue((v >> 16) & 0xFF); SetAlpha((v >> 24) & 0xFF); };
};

struct ColorMedium
{
    union
    {
        u16 color;

        struct
        {
            u16 r : 4;
            u16 g : 4;
            u16 b : 4;
            u16 a : 4;
        };
    };

    u32 GetRed()   const { return Convert<4, 8>(r); }
    u32 GetGreen() const { return Convert<4, 8>(g); }
    u32 GetBlue()  const { return Convert<4, 8>(b); }
    u32 GetAlpha() const { return Convert<4, 8>(a); }

    void SetRed(u32 v)   { r = Convert<8, 4>(v); }
    void SetGreen(u32 v) { g = Convert<8, 4>(v); }
    void SetBlue(u32 v)  { b = Convert<8, 4>(v); }
    void SetAlpha(u32 v) { b = Convert<8, 4>(v); }
    void SetColor(u32 v) { SetRed(v & 0xFF); SetGreen((v >> 8) & 0xFF); SetBlue((v >> 16) & 0xFF); SetAlpha((v >> 24) & 0xFF); };
};

struct ColorHigh
{
    union
    {
        u32 color;

        struct
        {
            u32 r : 8;
            u32 g : 8;
            u32 b : 8;
            u32 a : 8;
        };
    };
};

#define SMOL_ABGR(r, g, b, a) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
#define SMOL_ARGB(r, g, b, a) SMOL_ABGR((a),(b),(g),(r))
#define SMOL_RGBA(r, g, b, a) SMOL_ARGB((a),(r),(g),(b))

/// Each channel is f32 in LDR range [0; 1]
#define SMOL_COLOR(r, g, b, a) SMOL_RGBA((u32)((r) * 255.0f), (u32)((g) * 255.0f), (u32)((b) * 255.0f), (u32)((a) * 255.0f))

struct RenderState
{
    struct {
        Property<s32> x, y, width, height;
        Property<f32> min_depth, max_depth, offset_x, offset_y;
    } viewport;

    struct {
        Property<s32> x, y, width, height;
    } scissor;

    struct {
        Property<GPUFillMode> wireframe;
        Property<GPUCullMode> culling;
        Property<bool> front_counter_clockwise, scissor;
    } rasterizer;

    struct {
        Property<bool> enable;
        Property<GPUBlend> src, dest, src_alpha, dest_alpha;
        Property<GPUBlendOp> op, op_alpha;
        Property<u8> write_mask = { GPUWriteMask_All };
    } blend;

    struct {
        Property<bool> depth_enable, depth_write, stencil_enable;
        Property<GPUCompareFunc> depth_compare;
        Property<u8> stencil_read_mask, stencil_write_mask, stencil_ref;

        // Front-Face Stencil
        Property<GPUStencilOp> ff_fail_op, ff_depth_fail_op, ff_pass_op;
        Property<GPUCompareFunc> ff_func;

        // Back-Face Stencil
        Property<GPUStencilOp> bf_fail_op, bf_depth_fail_op, bf_pass_op;
        Property<GPUCompareFunc> bf_func;
    } depth;

    struct {
        Property<f32> near, far;
        Property<u32> color;
    } fog;

    Property<bool> dither, alpha_test, shade_smooth;
    Property<u32> env_color;
    Property<GPUTopology> topology;
};

class DataAlignedPool {
public:
    void* operator new(size_t);
    void* operator new[](size_t);
    void* operator new(size_t, const std::nothrow_t& tag);
    void* operator new[](size_t, const std::nothrow_t& tag);
    //void* operator new(size_t, void*);

public:
    

};

class BaseResource: public DataAlignedPool {
protected:
    String debug_name = "unnamed";

public:
    void SetDebugName(StringView name) { debug_name = name.data(); }
    const StringView& GetDebugName() const { return debug_name; }
};

struct Data
{
    void* ptr;
    size_t size;
};

class VertexBuffer: public BaseResource {
private:
    friend class Pipeline;

    enum { MAX_VERTEX_DATA_PER_VERTEX = 8 };
    VertexFormat format; //[MAX_VERTEX_DATA_PER_VERTEX];
    u32 format_length = 1;
    u32 vertex_count = 0;
    u64 format_psp = 0;
    u32 stride = 0;
    Data data;

    void update_format_psp();

public:
    void Create(u32 vcount, VertexFormat format, const Data& src, u32 format_len=1);
    void Release();

    void Bind();
    void Unbind();

    u64 GetFormatPSP() const { return format_psp; }
    u32 GetVertexCount() const { return vertex_count; }
    const void* GetData() const { return data.ptr; }
    u32 GetStride() const { return stride; }


    //void Copy(VertexBuffer* src, u32 start=0, u32 len=-1);
    //void Copy(VertexBuffer* src);

};

class IndexBuffer: public BaseResource {
private:
    friend class Pipeline;
    u32 index_count = 0;
    bool highquality = false;
    Data data = nullptr;

public:
    u32 GetIndexCount() const { return index_count; }
    const void* GetData() const { return data; }
    bool IsHightQuality() const { return highquality; }

};

class BoundVolume {
private:

public:
    virtual u32 GetVertexNum() const { return 0; }
    virtual f32* GetVertices() { return nullptr; }
};

class Mesh: public BaseResource {
private:
    BoundVolume* bound = nullptr;
    VertexBuffer* vb = nullptr;
    IndexBuffer* ib = nullptr;



public:
    void Create(VertexBuffer* vb, IndexBuffer* ib=nullptr);
    void Draw(bool transform_3d=true);


};

class Texture: public BaseResource {
private:
    friend class Pipeline;

    /// PSP: 
    /// 0: Block-aligned size
    /// 1: RAM Texture Data
    /// 3: Read only flag
    void* private_data[4]{ 0, nullptr, 0, 0 };
    TextureQuality quality;
    bool is_depth = false;
    u32 flags, width, height;

public:


};

class BoundBox final: public BoundVolume {
private:
    enum { VERTEX_COUNT = 2 };
    f32 vertices[VERTEX_COUNT * 3];

    bool is_touched();
    void update_vertices();

public:
    Property<f32> x, y, z, width, height, depth;
    
    virtual u32 GetVertexNum() const override { return VERTEX_COUNT; }
    virtual f32* GetVertices() override {
        if( is_touched() ) 
            update_vertices();

        return vertices;
    }
};

class Pipeline {
private:
    static Texture* backbuffer;
    static Texture* backbuffer_depth;
    static bool viewport, viewport_offset, scissor, raster, blend, depth, fog;

    static void InternalInit();
    static void CheckStates();
    static void PostSubmitStates();

    friend void EngineRun(class Game* game);
    static void Present();

public:
    static void Init();
    static void SubmitState();

    static Texture* GetBackbuffer() { return backbuffer; }
    static Texture* GetBackbufferDepth() { return backbuffer_depth; }

    static Texture* CreateTexture(u32 width, u32 height, TextureQuality quality, void* data = nullptr);

    static Texture* CreateBufferTexture(u32 width, u32 height, TextureQuality quality);
    static Texture* CreateBufferTexture(Texture* texture, u32 width, u32 height, TextureQuality quality);

    static void SetTarget(Texture* target);
    static void ResetTarget();

    static void ClearDepth(f32 clear_value);
    static void ClearStencil(u8 clear_value);
    static void ClearColor(u32 clear_value);

    static void Draw();

    //static Stack<RenderState*> render_state;
    static RenderState* render_state;
};
