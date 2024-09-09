#include "WeaponReward.h"

void UWeaponReward::SetDescriptionByLevel(const int32& NextLevel)
{
    switch (NextLevel)
    {
    case 1:
        Description = DescLV1;
        break;
    case 2:
        Description = DescLV2;
        break;
    case 3:
        Description = DescLV3;
        break;
    case 4:
        Description = DescLV4;
        break;
    case 5:
        Description = DescLV5;
        break;
    case 6:
        Description = DescLV6;
        break;
    case 7:
        Description = DescLV7;
        break;
    default:
        Description = "Err: Already Max Level";
        break;
    }
}