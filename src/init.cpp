#include "../testing_h/logger.h"
#include "./init.h"
#include "./ui/abstractmodels/playermodel.h"

void init()
{
    lprintf(LOG_INFO, "Initialising shared icons\n");
    player_model_init_icons();
}
