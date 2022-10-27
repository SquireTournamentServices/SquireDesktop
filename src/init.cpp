#include "../testing_h/logger.h"
#include "./init.h"
#include "./ui/abstractmodels/playermodel.h"
#include "./ui/widgets/recenttournamentwidget.h"

void init()
{
    lprintf(LOG_INFO, "Initialising shared icons\n");
    player_model_init_icons();

    recent_tourn_widget_init_icons();
}
