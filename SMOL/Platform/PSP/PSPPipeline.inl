#include "Platform/Pipeline.h"
#include "Misc/Utils.h"
#include "Misc/Log.h"

#include <pspgu.h>
#include <pspdisplay.h>

#define PSP_LINE_SIZE       (512)
#define PIXEL_SIZE          (4)
#define SCREEN_WIDTH        (480)
#define SCREEN_HEIGHT       (272)
#define FRAMEBUFFER_SIZE    (PSP_LINE_SIZE * SCREEN_WIDTH * PIXEL_SIZE)

#if SMOL_OS == SMOL_OS_PSP
    #define ALIGNED(x) __attribute__((aligned(x)))
#else
    #define ALIGNED(x) __declspec(align(x))
#endif

static unsigned int ALIGNED(16) list[262144];

void Pipeline::InternalInit() {
    RenderState* rs = render_state;
    rs->viewport.x = 2048;
    rs->viewport.y = 2048;
    rs->viewport.width  = SCREEN_WIDTH;
    rs->viewport.height = SCREEN_HEIGHT;
    rs->viewport.offset_x = 2048 - (SCREEN_WIDTH / 2);
    rs->viewport.offset_y = 2048 - (SCREEN_HEIGHT / 2);

    sceGuInit();
    sceGuStart(GU_DIRECT, list);

    backbuffer->quality = TextureQuality::High;
    backbuffer->width  = SCREEN_WIDTH;
    backbuffer->height = SCREEN_HEIGHT;
    backbuffer->private_data[0] = (void*)FRAMEBUFFER_SIZE;

    backbuffer_depth->width = SCREEN_WIDTH;
    backbuffer_depth->height = SCREEN_HEIGHT;
    backbuffer_depth->is_depth = true;
    backbuffer_depth->private_data[0] = (void*)(FRAMEBUFFER_SIZE * 2);

    SetTarget(backbuffer);
    SetTarget(backbuffer_depth);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, (void*)0, PSP_LINE_SIZE);
    ClearDepth(1.0f);

    rs->depth.depth_compare = GPUCompareFunc::LEQ;
    rs->blend.op = GPUBlendOp::Add;
    rs->blend.src = GPUBlend::SrcAlpha;
    rs->blend.dest = GPUBlend::InvSrcAlpha;

    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_LINEAR, GU_LINEAR);
    sceGuShadeModel(GU_FLAT);

    rs->rasterizer.culling = GPUCullMode::None;
    rs->dither = true;

    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

u32 convert(TextureQuality quality) {
    if( quality == TextureQuality::LowNoAlpha ) return GU_PSM_5650;
    if( quality == TextureQuality::LowAlpha   ) return GU_PSM_5551;
    if( quality == TextureQuality::Medium     ) return GU_PSM_4444;
    if( quality == TextureQuality::High       ) return GU_PSM_8888;

    if( quality == TextureQuality::DXT1 ) return GU_PSM_DXT1;
    if( quality == TextureQuality::DXT3 ) return GU_PSM_DXT3;
    if( quality == TextureQuality::DXT5 ) return GU_PSM_DXT5;

    return GU_PSM_5650;
}

u32 convert(GPUStencilOp op) {
    if( op == GPUStencilOp::Keep    ) return GU_KEEP;
    if( op == GPUStencilOp::Zero    ) return GU_ZERO;
    if( op == GPUStencilOp::Replace ) return GU_REPLACE;
    if( op == GPUStencilOp::Inc     ) return GU_INCR;
    if( op == GPUStencilOp::Inc_sat ) return GU_INCR;
    if( op == GPUStencilOp::Dec     ) return GU_DECR;
    if( op == GPUStencilOp::Dec_sat ) return GU_DECR;
    if( op == GPUStencilOp::Invert  ) return GU_INVERT;

    return GU_KEEP;
}

u32 convert(GPUBlendOp op) {
    if( op == GPUBlendOp::Add    ) return GU_ADD;
    if( op == GPUBlendOp::Sub    ) return GU_SUBTRACT;
    if( op == GPUBlendOp::RevSub ) return GU_REVERSE_SUBTRACT;
    if( op == GPUBlendOp::Min    ) return GU_MIN;
    if( op == GPUBlendOp::Max    ) return GU_MAX;

    return GU_ADD;
}

u32 convert(GPUBlend blend) {
    if( blend == GPUBlend::SrcColor     ) return GU_SRC_COLOR;
    if( blend == GPUBlend::InvSrcColor  ) return GU_ONE_MINUS_SRC_COLOR;
    if( blend == GPUBlend::SrcAlpha     ) return GU_SRC_ALPHA;
    if( blend == GPUBlend::InvSrcAlpha  ) return GU_ONE_MINUS_SRC_ALPHA;
    if( blend == GPUBlend::DestAlpha    ) return GU_DST_ALPHA;
    if( blend == GPUBlend::InvDestAlpha ) return GU_ONE_MINUS_DST_COLOR;
    if( blend == GPUBlend::DestColor    ) return GU_DST_COLOR;
    if( blend == GPUBlend::InvDestColor ) return GU_ONE_MINUS_DST_COLOR;

    return GU_SRC_COLOR;
}

u32 convert(GPUCompareFunc func) {
    if( func == GPUCompareFunc::Always   ) return GU_ALWAYS;
    if( func == GPUCompareFunc::Never    ) return GU_NEVER;
    if( func == GPUCompareFunc::Less     ) return GU_LESS;
    if( func == GPUCompareFunc::LEQ      ) return GU_LEQUAL;
    if( func == GPUCompareFunc::Equal    ) return GU_EQUAL;
    if( func == GPUCompareFunc::Greater  ) return GU_GREATER;
    if( func == GPUCompareFunc::GEQ      ) return GU_GEQUAL;
    if( func == GPUCompareFunc::NotEqual ) return GU_NOTEQUAL;

    return GU_ALWAYS;
}

u32 convert(GPUTopology topo) {
    if( topo  == GPUTopology::LineList      ) return GU_LINES;
    if( topo  == GPUTopology::LineStrip     ) return GU_LINE_STRIP;
    if( topo  == GPUTopology::TriangleList  ) return GU_TRIANGLES;
    if( topo  == GPUTopology::TriangleStrip ) return GU_TRIANGLE_STRIP;
    if( topo  == GPUTopology::QuadList      ) return GU_SPRITES;

    return GU_TRIANGLES;
}

template<typename T>
void sceGuToggle(const Property<T>& value, const T& disabled_value, u32 state) {
    if( value.IsChanged() == false )
        return;

    if( value == disabled_value )
        sceGuDisable(state);
    else
        sceGuEnable(state);
}

void Pipeline::SubmitState() {
    CheckStates();

    RenderState* rs = render_state;

    if( viewport ) {
        sceGuViewport(rs->viewport.x, rs->viewport.y, rs->viewport.width, rs->viewport.height);
        sceGuDepthRange(ToU32(rs->viewport.min_depth * 65535.0f), ToU32(rs->viewport.max_depth * 65535.0f));
    }

    if( viewport_offset )
        sceGuOffset(rs->viewport.offset_x, rs->viewport.offset_y);

    if( scissor ) 
        sceGuScissor(rs->scissor.x, rs->scissor.y, rs->scissor.x + rs->scissor.width, rs->scissor.y + rs->scissor.height);
    
    if( raster ) {
        sceGuToggle(rs->rasterizer.culling, GPUCullMode::None, GU_CULL_FACE);
        sceGuToggle(rs->rasterizer.scissor, false, GU_SCISSOR_TEST);

        if( rs->rasterizer.front_counter_clockwise.IsChanged() )
            sceGuFrontFace(rs->rasterizer.front_counter_clockwise ? GU_CCW : GU_CW);
    }
    
    if( depth ) {
        sceGuToggle(rs->depth.depth_enable, false, GU_DEPTH_TEST);
        sceGuDepthFunc(convert(rs->depth.depth_compare.get()));
        sceGuStencilFunc(convert(rs->depth.ff_func.get()), rs->depth.stencil_ref, rs->depth.stencil_read_mask);
        sceGuStencilOp(convert(rs->depth.ff_fail_op.get()), convert(rs->depth.ff_depth_fail_op.get()), convert(rs->depth.ff_pass_op.get()));
    }

    if( blend ) {
        sceGuToggle(rs->blend.enable, false, GU_BLEND);
        sceGuBlendFunc(convert(rs->blend.op.get()), convert(rs->blend.src.get()), convert(rs->blend.dest.get()), 0, 0);
        //sceGuAlphaFunc(convert(rs->blend.op_alpha.get()), );
    }

    sceGuToggle(rs->dither, false, GU_DITHER);

    PostSubmitStates();
}

bool BoundBox::is_touched() {
    return x.IsChanged() || y.IsChanged() || z.IsChanged() || 
           width.IsChanged() || height.IsChanged() || depth.IsChanged();
}

void BoundBox::update_vertices() {
    vertices[0] = x;
    vertices[1] = y;
    vertices[2] = z;

    vertices[3] = x + width;
    vertices[4] = y + height;
    vertices[5] = z + depth;
}

void VertexBuffer::update_format_psp()
{
    format_psp = 0;
    stride = 0;

#define PSP_FORMAT(X, Y) { format_psp |= X; stride += Y; }
    //for( u32 i = 0; i < format_length; i++ )
    {
        VertexFormat& f = format;

        if( f & VertexFormat::VFVertex8  ) PSP_FORMAT(GU_VERTEX_8BIT, 1);
        if( f & VertexFormat::VFVertex16 ) PSP_FORMAT(GU_VERTEX_16BIT, 2);
        if( f & VertexFormat::VFVertex32 ) PSP_FORMAT(GU_VERTEX_32BITF, 4);

        if( f & VertexFormat::VFTexture8  ) PSP_FORMAT(GU_TEXTURE_8BIT, 1);
        if( f & VertexFormat::VFTexture16 ) PSP_FORMAT(GU_TEXTURE_16BIT, 2);
        if( f & VertexFormat::VFTexture32 ) PSP_FORMAT(GU_TEXTURE_32BITF, 4);

        if( f & VertexFormat::VFNormal8  ) PSP_FORMAT(GU_NORMAL_8BIT, 1);
        if( f & VertexFormat::VFNormal16 ) PSP_FORMAT(GU_NORMAL_16BIT, 2);
        if( f & VertexFormat::VFNormal32 ) PSP_FORMAT(GU_NORMAL_32BITF, 4);

        if( f & VertexFormat::VFColorLowNoAlpha  ) PSP_FORMAT(GU_COLOR_5650, 2);
        if( f & VertexFormat::VFColorLowAlpha    ) PSP_FORMAT(GU_COLOR_5551, 2);
        if( f & VertexFormat::VFColorMedium      ) PSP_FORMAT(GU_COLOR_4444, 2);
        if( f & VertexFormat::VFColorHigh        ) PSP_FORMAT(GU_COLOR_8888, 4);
    }
#undef PSP_FORMAT

    if( stride & 31 )
        Log::Warning("Vertex buffer %s is unaligned!", debug_name.data());
}

void VertexBuffer::Create(u32 vcount, VertexFormat fmt, const Data& src, u32 format_len)
{
    format = fmt;
    vertex_count = vcount;
    format_length = format_len;
    data = src;
    
    update_format_psp();
}

void Mesh::Draw(bool transform_3d) {
    u32 topology = convert(Pipeline::render_state->topology.get());

    if( !vb ) 
        return;

    u32 format = vb->GetFormatPSP() | (transform_3d ? GU_TRANSFORM_3D : GU_TRANSFORM_2D);
    if( bound && bound->GetVertexNum() ) sceGuBeginObject(GU_VERTEX_32BITF, bound->GetVertexNum(), nullptr, bound->GetVertices());
    {
        if( ib ) {
            format |= ib->IsHightQuality() ? GU_INDEX_16BIT : GU_INDEX_8BIT;
            sceGuDrawArray(topology, format, vb->GetVertexCount(), ib->GetData(), vb->GetData());
        } else {
            sceGuDrawArray(topology, format, vb->GetVertexCount(), nullptr, vb->GetData());
        }
    }
    if( bound && bound->GetVertexNum() ) sceGuEndObject();
}

void Pipeline::Present() {
    sceGuFinish();
    sceGuSync(0, 0);
}

Texture* Pipeline::CreateTexture(u32 width, u32 height, TextureQuality quality, void* data) {
    Texture* texture = new Texture();
    texture->width = width;
    texture->height = height;
    texture->quality = quality;
    texture->is_depth = false;
    texture->private_data[0] = nullptr;
    texture->private_data[1] = data;
    texture->private_data[2] = (data != nullptr) ? (void*)true : (void*)false;

    return texture;
}

Texture* Pipeline::CreateBufferTexture(u32 width, u32 height, TextureQuality quality) {
    Texture* texture = new Texture();
    return CreateBufferTexture(texture, width, height, quality);
}

Texture* Pipeline::CreateBufferTexture(Texture* texture, u32 width, u32 height, TextureQuality quality) {
    texture->width = width;
    texture->height = height;
    texture->quality = quality;
    texture->is_depth = (quality >= TextureQuality::Depth16) && (quality <= TextureQuality::Depth32);
    texture->private_data[0] = (void*)(FRAMEBUFFER_SIZE * 2 + PSP_LINE_SIZE * PIXEL_SIZE * width);
    texture->private_data[1] = nullptr;
    texture->private_data[2] = (void*)false;

    return texture;
}

void Pipeline::SetTarget(Texture* target) {
    /// Can't write to read only textures
    if( target->private_data[2] )
        return;

    if( target->is_depth ) {
        sceGuDepthBuffer(target->private_data[0], PSP_LINE_SIZE);
    } else {
        sceGuDrawBuffer(convert(target->quality), target->private_data[0], PSP_LINE_SIZE);
    }
}

void Pipeline::ClearDepth(f32 clear_value) {
    sceGuClearDepth(ToF32(clear_value * 65535.0f));
}
