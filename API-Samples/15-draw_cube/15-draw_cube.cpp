/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2020 Valve Corporation
 * Copyright (C) 2015-2020 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
VULKAN_SAMPLE_SHORT_DESCRIPTION
Draw Cube
*/

/* This is part of the draw cube progression */

#include <util_init.hpp>
#include <assert.h>
#include <string.h>
#include <cstdlib>
#include "cube_data.h"

/* We've setup cmake to process 15-draw_cube.vert and 15-draw_cube.frag                   */
/* files containing the glsl shader code for this sample.  The generate-spirv script uses */
/* glslangValidator to compile the glsl into spir-v and places the spir-v into a struct   */
/* into a generated header file                                                           */

int sample_main(int argc, char *argv[]) {
    VkResult U_ASSERT_ONLY res;
    struct sample_info info = {};
    char sample_title[] = "Draw Cube";
    const bool depthPresent = true;

    process_command_line_args(info, argc, argv);
    init_global_layer_properties(info);
    init_instance_extension_names(info);
    init_device_extension_names(info);
    init_instance(info, sample_title);
    init_enumerate_device(info);
    init_window_size(info, 500, 500);
    init_connection(info);
    init_window(info);
    init_swapchain_extension(info);
    init_device(info);

    init_command_pool(info);
    init_command_buffer(info);

    init_device_queue(info);
    //init_swap_chain(info);
    //init_depth_buffer(info);
    //init_uniform_buffer(info);
    init_descriptor_and_pipeline_layouts(info, false);
    //init_renderpass(info, depthPresent);
#include "15-draw_cube.comp.h"

    VkShaderModuleCreateInfo comp_info = {};
    comp_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    comp_info.codeSize = sizeof(__draw_cube_comp);
    comp_info.pCode = __draw_cube_comp;

    /* -----------------------init_shaders-------------------------------*/
    std::vector<unsigned int> vtx_spv;
    info.shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.shaderStages[0].pNext = NULL;
    info.shaderStages[0].pSpecializationInfo = NULL;
    info.shaderStages[0].flags = 0;
    info.shaderStages[0].stage = VK_SHADER_STAGE_COMPUTE_BIT;
    info.shaderStages[0].pName = "main";
    res = vkCreateShaderModule(info.device, &comp_info, NULL, &info.shaderStages[0].module);
    assert(res == VK_SUCCESS);


    //init_shaders(info, &vert_info, &frag_info);
    //init_framebuffers(info, depthPresent);
//    init_vertex_buffer(info, g_vb_solid_face_colors_Data, sizeof(g_vb_solid_face_colors_Data),
//                       sizeof(g_vb_solid_face_colors_Data[0]), false);
    init_descriptor_pool(info, false);
    init_descriptor_set(info, false);
    init_pipeline_cache(info);
    init_pipeline(info, depthPresent);

    /* VULKAN_KEY_START */
    execute_begin_command_buffer(info);
    vkCmdBindPipeline(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline);
    vkCmdBindDescriptorSets(info.cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, info.pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
                            info.desc_set.data(), 0, NULL);
    vkCmdDispatch(info.cmd, 1, 1, 1);

    res = vkEndCommandBuffer(info.cmd);
    const VkCommandBuffer cmd_bufs[] = {info.cmd};

    VkFenceCreateInfo fenceInfo;
    VkFence drawFence;
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = NULL;
    fenceInfo.flags = 0;
    vkCreateFence(info.device, &fenceInfo, NULL, &drawFence);

    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info[1] = {};
    submit_info[0].pNext = NULL;
    submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info[0].waitSemaphoreCount = 0;
    submit_info[0].pWaitSemaphores = nullptr;
    submit_info[0].pWaitDstStageMask = nullptr;
    submit_info[0].commandBufferCount = 1;
    submit_info[0].pCommandBuffers = cmd_bufs;
    submit_info[0].signalSemaphoreCount = 0;
    submit_info[0].pSignalSemaphores = NULL;

    /* Queue the command buffer for execution */
    res = vkQueueSubmit(info.graphics_queue, 1, submit_info, drawFence);
    assert(res == VK_SUCCESS);

    /* Make sure command buffer is finished before presenting */
    do {
        res = vkWaitForFences(info.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
    } while (res == VK_TIMEOUT);

    wait_seconds(1);
    /* VULKAN_KEY_END */
    return 0;
}
