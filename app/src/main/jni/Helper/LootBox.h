const char *GetPickUpName(APlayerTombBox *APickUpListWrapperActor)
{
    switch (APickUpListWrapperActor->BoxType)
    {
    case EPickUpBoxType::EPickUpBoxType__EPickUpBoxType_TombBox:
        return "DeadBox";
        break;
    case EPickUpBoxType::EPickUpBoxType__EPickUpBoxType_AirDropBox:
        return "AirDrop";
        break;
    case EPickUpBoxType::EPickUpBoxType__EPickUpBoxType_TreasureBox:
        return "TreasureBox";
        break;
    case EPickUpBoxType::EPickUpBoxType__EPickUpBoxType_MonsterTombBox:
        return "MonsterDeadBox";
        break;
    case EPickUpBoxType::EPickUpBoxType__EPickUpBoxType_VehicleBox:
        return "VehicleBox";
        break;
    case EPickUpBoxType::EPickUpBoxType__EPickUpBoxType_MAX:
        return "MAX";
        break;
    default:
        return "LootBox";
        break;
    }
    return "LootBox";
}
