#include "pc.h"
#include "Pipeline.h"

#include "Platform/Window.h"

//Stack<RenderState*> Pipeline::render_state = {};
RenderState* Pipeline::render_state = nullptr;
Texture* Pipeline::backbuffer_depth = nullptr;
Texture* Pipeline::backbuffer = nullptr;

bool Pipeline::viewport, 
    Pipeline::viewport_offset, 
    Pipeline::scissor, 
    Pipeline::raster, 
    Pipeline::blend, 
    Pipeline::depth, 
    Pipeline::fog;

#if SMOL_OS == SMOL_OS_WIN
    #include "Win32/WinPipeline.inl"
#elif SMOL_OS == SMOL_OS_PSP
    #include "PSP/PSPPipeline.inl"
#elif SMOL_OS == SMOL_OS_SWITCH
    #include "Switch/SwitchPipeline.inl"
#endif

void Pipeline::Init() {
    render_state     = new RenderState();
    backbuffer       = new Texture();
    backbuffer_depth = new Texture();

    RenderState* rs = render_state;

    rs->viewport.x = 0.0f;
    rs->viewport.y = 0.0f;
    rs->viewport.width  = Window::GetWidth();
    rs->viewport.height = Window::GetHeight();
    rs->viewport.min_depth = 1.0f;
    rs->viewport.max_depth = 0.0f;
    rs->viewport.offset_x = 2048 - (480 / 2);
    rs->viewport.offset_y = 2048 - (272 / 2);

    rs->scissor.x = 0;
    rs->scissor.y = 0;
    rs->scissor.width  = Window::GetWidth();
    rs->scissor.height = Window::GetHeight();

    InternalInit();
}

void Pipeline::CheckStates() {
    RenderState* rs = render_state;
    
    viewport = rs->viewport.x.IsChanged() || rs->viewport.y.IsChanged()
        || rs->viewport.width.IsChanged() || rs->viewport.height.IsChanged()
        || rs->viewport.min_depth.IsChanged() || rs->viewport.max_depth.IsChanged();

    viewport_offset = rs->viewport.offset_x.IsChanged() || rs->viewport.offset_y.IsChanged();

    scissor = rs->scissor.x.IsChanged() || rs->scissor.y.IsChanged()
        || rs->scissor.width.IsChanged() || rs->scissor.height.IsChanged();

    raster = rs->rasterizer.wireframe.IsChanged() || rs->rasterizer.culling.IsChanged()
        || rs->rasterizer.front_counter_clockwise.IsChanged() || rs->rasterizer.scissor.IsChanged();

    blend = rs->blend.enable.IsChanged() || rs->blend.src.IsChanged()
        || rs->blend.dest.IsChanged() || rs->blend.src_alpha.IsChanged() || rs->blend.write_mask.IsChanged()
        || rs->blend.dest_alpha.IsChanged() || rs->blend.op.IsChanged() || rs->blend.op_alpha.IsChanged();

    depth = rs->depth.depth_enable.IsChanged() || rs->depth.depth_write.IsChanged() || rs->depth.stencil_enable.IsChanged() || rs->depth.stencil_ref.IsChanged()
        || rs->depth.depth_compare.IsChanged() || rs->depth.stencil_read_mask.IsChanged() || rs->depth.stencil_write_mask.IsChanged()
        || rs->depth.ff_fail_op.IsChanged() || rs->depth.ff_depth_fail_op.IsChanged() || rs->depth.ff_pass_op.IsChanged() || rs->depth.ff_func.IsChanged()
        || rs->depth.bf_fail_op.IsChanged() || rs->depth.bf_depth_fail_op.IsChanged() || rs->depth.bf_pass_op.IsChanged() || rs->depth.bf_func.IsChanged();

    fog = rs->fog.near.IsChanged() || rs->fog.far.IsChanged() || rs->fog.color.IsChanged();


}

void Pipeline::PostSubmitStates() {
    RenderState* rs = render_state;

    rs->viewport.x.Reset();
    rs->viewport.y.Reset();
    rs->viewport.width.Reset();
    rs->viewport.height.Reset();
    rs->viewport.min_depth.Reset();
    rs->viewport.max_depth.Reset();
    rs->viewport.offset_x.Reset();
    rs->viewport.offset_y.Reset();

    rs->scissor.x.Reset();
    rs->scissor.y.Reset();
    rs->scissor.width.Reset();
    rs->scissor.height.Reset();

    rs->rasterizer.culling.Reset();
    rs->rasterizer.scissor.Reset();
    rs->rasterizer.wireframe.Reset();
    rs->rasterizer.front_counter_clockwise.Reset();

    rs->blend.op.Reset();
    rs->blend.src.Reset();
    rs->blend.dest.Reset();
    rs->blend.enable.Reset();
    rs->blend.src_alpha.Reset();
    rs->blend.write_mask.Reset();
    rs->blend.dest_alpha.Reset();
    rs->blend.op_alpha.Reset();

    rs->depth.bf_func.Reset();
    rs->depth.ff_func.Reset();
    rs->depth.bf_fail_op.Reset();
    rs->depth.ff_pass_op.Reset();
    rs->depth.bf_pass_op.Reset();
    rs->depth.ff_fail_op.Reset();
    rs->depth.depth_write.Reset();
    rs->depth.stencil_ref.Reset();
    rs->depth.depth_enable.Reset();
    rs->depth.depth_compare.Reset();
    rs->depth.stencil_enable.Reset();
    rs->depth.stencil_read_mask.Reset();
    rs->depth.stencil_write_mask.Reset();
    rs->depth.ff_depth_fail_op.Reset();
    rs->depth.bf_depth_fail_op.Reset();


}

void Mesh::Create(VertexBuffer* v, IndexBuffer* i)
{
    //vb = v;
    //ib = i;

    // Generate bound box

}
