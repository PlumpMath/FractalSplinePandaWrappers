// necessary so this whole thing links ok.

#include "pandaNode.h"

CollideMask PandaNode::
get_legal_collide_mask() const {
  return CollideMask::all_off();
}
