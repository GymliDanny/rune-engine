#ifndef VKCONTEXT_H
#define VKCONTEXT_H

#include "vk_types.h"

struct vkcontext* create_vkcontext(struct vklayer_container *vklayers, struct ext_container *ext);
void destroy_vkcontext(struct vkcontext *context);

struct vklayer_container* init_vklayers(struct ext_container *ext);

#endif
