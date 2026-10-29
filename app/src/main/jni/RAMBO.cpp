#include "Includes.h"
#include "Helper/obfuscate.h"
#include "Helper/fake_dlfcn.h"
#include "Helper/plthook.h"

#include "Helper/json.hpp"
using json = nlohmann::json;

#include "Tools.h"
#include "SDK.hpp"
using namespace SDK;

#include "Helper/Items.h"
#include "Helper/Vehicle.h"
#include "Helper/LootBox.h"
#include "Helper/Vector2.hpp"
#include "Helper/Vector3.hpp"
#include "KittyMemory/MemoryPatch.h"

#include "FloatingLogoTexture.h"


// ======================================================================== //
int screenWidth = -1, glWidth, screenHeight = -1, glHeight;
float density = -1;

json items_data;
time_t rng = 0;
bool initImGui = false;
bool FASTSHOOT = true;
float ControlSpeed = 0.25f;


enum EAimType {
    Distance = 0,
    Croshhair = 1
};

enum EAimTarget {
    Head = 0,
    Chest = 1
};

enum EAimTrigger {
    None = 0,
    Shooting = 1,
    Scoping = 2,
    Both = 3,
    Any = 4
	
};

std::map<int, bool> Items;
struct sConfig 
{
    //bool Logo;
    bool Box;
    bool LootI;
    
    
    
    struct sESPMenu 
    {
        
        bool Active;
        bool Line;
        bool Box;
        bool Skeleton;
        bool Unlimted;
        bool Health;
        bool TeamID;
        bool Name;
        bool Distance;
        bool Alert;
        bool Radar;
        float RadarX;
        float RadarY;
        bool Vehicle;
        bool LootBox;
        bool EnemyWeapon;
        bool GameInfo;
        bool OnScreen;
       // bool Enemy = true;
    };
    sESPMenu ESPMenu;
    
      struct sKittyMemory
    {
    bool Iceland;
    bool Lobby;
    };
    sKittyMemory KittyMemory;
    
    
    struct sAimMenu
    {
        bool Enable;
        bool AutoFire;
        float FOVSize;
        EAimType Type;
        EAimTarget Target;
        EAimTrigger Trigger;
        bool IgnoreKnocked;
        bool IgnoreBot;
        bool VisCheck;
    };
    sAimMenu CalcShootRot{0};
    sAimMenu AimMenu{0};
    sAimMenu AimBot{0};
    
    struct sHighRisk 
    {
        bool Recoil;
        bool Crosshair;
        bool Shake;
        bool HitEffect;
        bool Instant;
        bool Flash;
        bool Flash1;
        bool UnlimitedAmmo;
		bool UnlimitedAmmo1;
        bool Parachute;
        bool WideView;
        bool NoFog;
        bool KillMsg;
        bool STUCK;
        bool FLY;
		bool SkyFly;
        bool Jump;
        bool Prone;
    };
    sHighRisk HighRisk{0};

    struct sColorsESP 
    {
        float *WindowBG;
        float *FrameBG;
        float *FCircle;
        float *Color;
    };
    sColorsESP ColorsESP{0};

};
sConfig Config{0};

struct sPatches { 
MemoryPatch
Flash01,
Flash02,
Flash03,
Flash04,
Flash05,
ipad1,
ipad2,
Iceland1,
Iceland2,
Xhit,
Xhit1;
};
sPatches Patches;

class Rect {
public:
    float x;
    float y;
    float width;
    float height;

    Rect() {
        this->x = 0;
        this->y = 0;
        this->width = 0;
        this->height = 0;
    }

    Rect(float x, float y, float width, float height) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
    }

    bool operator==(const Rect &src) const {
        return (src.x == this->x && src.y == this->y && src.height == this->height &&
                src.width == this->width);
    }

    bool operator!=(const Rect &src) const {
        return (src.x != this->x && src.y != this->y && src.height != this->height &&
                src.width != this->width);
    }
};



#define CREATE_COLOR(r, g, b, a) new float[4] {(float)r, (float)g, (float)b, (float)a};
// ======================================================================== //
uintptr_t UE4 = 0;
uintptr_t ANOGS = 0;
android_app *g_App = 0;

ASTExtraPlayerCharacter *g_LocalPlayer = 0;
ASTExtraPlayerController *g_LocalController = 0;

#define GWorld_Offset 0x70BD278
#define GNames_Offset 0x2725EAC
#define GUObject_Offset 0x6FB4B10
#define GNativeApp_Offset 0x6DDE9C8
#define Actors_Offset 0xA0
#define ProcessEvent_Offset 0x294173C

namespace Active
{
inline int lastCarID = 0;
inline int newCarID = 0;
}

static int xsuitsolo = 0;

static int skinxsuit = 1;
static int xsuit4 = 1;
static int skinm4 = 1;
static int skinm16 = 1;
static int skinakm = 1;
static int skinscar = 1;
static int skinBAG = 1;
static int skinHELMET = 1;
static int skinTasa = 1;
static int skinawm = 1;
static int skingroza = 1;
static int skinm7 = 1;
static int skinuzi = 1;
static int skinump45 = 1;
static int skinvector = 1;
static int skintemson = 1;
static int skinkar = 1;
static int skinm24 = 1;
static int skinseug = 1;
static int skinsqp = 1;
static int skinsm = 1;
static int skinskss = 1;
static int skinsvss = 1;
static int skinsmin = 1;
static int skinsmk = 1;
static int skinslr = 1;
static int skins686 = 1;
static int skins189 = 1;
static int skinsdp = 1;
static int skinb92 = 1;
static int skinb19 = 1;
static int skinr18 = 1;
static int skinfleer = 1;
static int skinwin94 = 1;
static int skincropar = 1;
static int skinskil = 1;
static int skinm259 = 1;
static int skinparshor = 1;

static int UMP45 = 1;
static int Kar98K = 1;
static int AWM = 1;



struct sRegion {
    uintptr_t start, end;
};

std::vector<sRegion> trapRegions;

bool isObjectInvalid(UObject *obj) {
    if (!Tools::IsPtrValid(obj)) {
        return true;
    }

    if (!Tools::IsPtrValid(obj->ClassPrivate)) {
        return true;
    }

    if (obj->InternalIndex <= 0) {
        return true;
    }

    if (obj->NamePrivate.ComparisonIndex <= 0) {
        return true;
    }

    if ((uintptr_t)(obj) % sizeof(uintptr_t) != 0x0 && (uintptr_t)(obj) % sizeof(uintptr_t) != 0x4) {
        return true;
    }

    if (std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t) obj) >= region.start && ((uintptr_t) obj) <= region.end; }) ||
        std::any_of(trapRegions.begin(), trapRegions.end(), [obj](sRegion region) { return ((uintptr_t) obj->ClassPrivate) >= region.start && ((uintptr_t) obj->ClassPrivate) <= region.end; })) {
        return true;
    }

    return false;
}

TNameEntryArray *GetGNames(){
    return ((TNameEntryArray *(*)()) (UE4 + GNames_Offset))();
}

TArray<AActor *> getActors() 
{
    auto World = *(UWorld **)(UE4 + GWorld_Offset);
    if (World) {
        auto PersistentLevel = World->PersistentLevel;
        if (PersistentLevel) {
            auto GetActorArray = ((uintptr_t) PersistentLevel + Actors_Offset);
            if (GetActorArray) {
                return *(TArray<AActor *> *) ((uintptr_t) GetActorArray);
            }
        }
    }
    return TArray<AActor *>();
}
// ======================================================================== //
std::string getObjectPath(UObject *Object) {
    std::string s;
    for (auto super = Object->ClassPrivate; super; super = (UClass *) super->SuperStruct) {
        if (!s.empty())
            s += ".";
        s += super->NamePrivate.GetName();
    }
    return s;
}

const char *getStringName(UObject *Object) {
    std::string s;
    for (auto super = Object->ClassPrivate; super; super = (UClass *) super->SuperStruct) {
        if (!s.empty())
            s += ".";
        s += super->GetName();
    }
    return s.c_str();
}

bool WriteAddr(void *addr, void *buffer, size_t length) {
    unsigned long page_size = sysconf(_SC_PAGESIZE);
    unsigned long size = page_size * sizeof(uintptr_t);
    return mprotect((void *) ((uintptr_t) addr - ((uintptr_t) addr % page_size) - page_size), (size_t) size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0 && memcpy(addr, buffer, length) != 0;
}

template<typename T>
void Write(uintptr_t addr, T value) {
    WriteAddr((void *) addr, &value, sizeof(T));
}

int Write_FLOAT(long int addr, float value){
    Tools::WriteAddr((void*)(addr), (void*)&value , 4);
    return 0;
}

int Write_DWORD(long int addr, int value){
    Tools::WriteAddr((void*)(addr),(void*)&value ,sizeof(value));
    return 0;
}

int Write_QWORD(long int addr, int64_t value){
    Tools::WriteAddr((void*)(addr),(void*)&value ,sizeof(value));
    return 0;
}
// ================================================ //
int32_t ToColor(float *col) {
    return ImGui::ColorConvertFloat4ToU32(*(ImVec4 *) (col));
}
//================================================= //
FRotator ToRotator(FVector local, FVector target) {
    FVector rotation = UKismetMathLibrary::Subtract_VectorVector(local, target);
    float hyp = sqrt(rotation.X * rotation.X + rotation.Y * rotation.Y);
    FRotator newViewAngle = {0};
    newViewAngle.Pitch = -atan(rotation.Z / hyp) * (180.f / (float) 3.14159265358979323846);
    newViewAngle.Yaw = atan(rotation.Y / rotation.X) * (180.f / (float) 3.14159265358979323846);
    newViewAngle.Roll = (float) 0.f;
    if (rotation.X >= 0.f)
        newViewAngle.Yaw += 180.0f;
    return newViewAngle;
}

bool isInsideFOV(int x, int y) {
    if (!Config.AimMenu.FOVSize)
        return true;

    int circle_x = glWidth / 2;
    int circle_y = glHeight / 2;
    int rad = Config.AimMenu.FOVSize;
    return (x - circle_x) * (x - circle_x) + (y - circle_y) * (y - circle_y) <= rad * rad;
}

#define W2S(w, s) UGameplayStatics::ProjectWorldToScreen(localController, w, true, s)
//=============≈=====BulletTrack360°=============//
auto GetTargetByDistance() {
    ASTExtraPlayerCharacter *result = 0;
    float max = std::numeric_limits<float>::infinity();
    
    auto localPlayer = g_LocalPlayer;
    auto localController = g_LocalController;
    auto Actors = getActors();
    
    FVector ViewPosY{0, 0, 0};
    if (localPlayer) {                   
       ViewPosY = localPlayer->GetBonePos("Head");
       ViewPosY.Z += 10.f;
    }

             if (localPlayer) {
                for (int i = 0; i < Actors.Num(); i++) {
                    auto Actor = Actors[i];
                    if (isObjectInvalid(Actor))
                        continue;

                    if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {
                        auto Player = (ASTExtraPlayerCharacter *) Actor;
                        
                        if (Player->PlayerKey == localController->PlayerKey)
                            continue;

                        if (Player->TeamID == localController->TeamID)
                            continue;

                        if (Player->bDead)
                            continue;
                            
                        if (Config.AimMenu.IgnoreBot) {
                            if (Player->bIsAI)
                                continue;
                        }
                        
                        if (Config.AimMenu.VisCheck) {
                            if (!localController->LineOfSightTo(Player, ViewPosY, true))
                                continue;
                        }

                        if (Config.AimMenu.IgnoreKnocked) {
                            if (Player->Health == 0.0f)
                                continue;
                        }
                            
                        float dist = g_LocalPlayer->GetDistanceTo(Player);
                        if (dist < max) {
                            max = dist;
                            result = Player;
                        }
                    }
                }
            }
    return result;
}

auto GetTargetByCrossDist() {
    ASTExtraPlayerCharacter *result = 0;
    float max = std::numeric_limits<float>::infinity();

    auto localPlayer = g_LocalPlayer;
    auto localController = g_LocalController;
    auto Actors = getActors();
    
    FVector ViewPosY{0, 0, 0};
    if (localPlayer) {                   
       ViewPosY = localPlayer->GetBonePos("Head");
       ViewPosY.Z += 10.f;
    }
    
            if (localPlayer) {
                for (int i = 0; i < Actors.Num(); i++) {
                    auto Actor = Actors[i];
                    if (isObjectInvalid(Actor))
                        continue;

                    if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {
                        auto Player = (ASTExtraPlayerCharacter *) Actor;
                        
                        if (Player->PlayerKey == localController->PlayerKey)
                            continue;

                        if (Player->TeamID == localController->TeamID)
                            continue;

                        if (Player->bDead)
                            continue;
                            
                        if (Config.AimMenu.IgnoreBot) {
                            if (Player->bIsAI)
                                continue;
                        }
                        
                        if (Config.AimMenu.VisCheck) {
                            if (!localController->LineOfSightTo(Player, ViewPosY, true)){
                                continue;
                        }}

                        if (Config.AimMenu.IgnoreKnocked) {
                            if (Player->Health == 0.0f)
                                continue;
                        }
                        
                        auto Root = Player->GetBonePos("Root");
                        auto Head = Player->GetBonePos("Head");

                        FVector2D RootSc, HeadSc;
                        if (W2S(Root, &RootSc) && W2S(Head, &HeadSc)) {
                            float height = abs(HeadSc.Y - RootSc.Y);
                            float width = height * 0.65f;

                            FVector middlePoint = {HeadSc.X + (width / 2), HeadSc.Y + (height / 2),
                                                   0};
                            if ((middlePoint.X >= 0 && middlePoint.X <= glWidth) &&
                                (middlePoint.Y >= 0 && middlePoint.Y <= glHeight)) {
                                FVector2D v2Middle = FVector2D((float) (glWidth / 2),
                                                               (float) (glHeight / 2));
                                FVector2D v2Loc = FVector2D(middlePoint.X, middlePoint.Y);

                                if (isInsideFOV((int) middlePoint.X, (int) middlePoint.Y)) {
                                float dist = FVector2D::Distance(v2Middle, v2Loc);

                                if (dist < max) {
                                    max = dist;
                                    result = Player;
                                }
                            }
                        }
                    }
                }
            }
        }
    return result;
}
//==================================================================//
ASTExtraPlayerCharacter *GetTargetForAim() {
    if (Config.AimMenu.Type == EAimType::Distance) {
        return GetTargetByDistance();
    } else if (Config.AimMenu.Type == EAimType::Croshhair) {
        return GetTargetByCrossDist();
    }
    return 0;
}




bool qwcifqvs86y8fify = false;
//==================================================================//
void (*orig_shoot_event)(USTExtraShootWeaponComponent *Thiz, FVector start, FRotator rot, void *unk1, int unk2) = 0;
void shoot_event(USTExtraShootWeaponComponent *thiz, FVector start, FRotator rot, ASTExtraShootWeapon *weapon, int unk1)
{

    if (Config.CalcShootRot.Enable)
    {
        ASTExtraPlayerCharacter *Target = GetTargetForAim();


        if (Target)
        {
            FVector targetAimPos = Target->GetBonePos("Head");
            targetAimPos.Z -= -25.0f;
            FRotator sex = ToRotator(start, targetAimPos);
            return orig_shoot_event(thiz, targetAimPos, sex, weapon, unk1);
        }
    }
    return orig_shoot_event(thiz, start, rot, weapon, unk1);
}

float (*sub_3503040)(unsigned long long a1, int a2, float a3);
float hsub_3503040(unsigned long long a1, int a2, float a3) {
    return fmaxf(a3, 0.3f);
}

float (*BAM_3503040)(unsigned long long a1, int a2, float a3);
float YT_3503040(unsigned long long a1, int a2, float a3) {
    return (a3);
}

// ============ BT ===========//

FRotator (*oCalcShootRot)(USTExtraShootWeaponComponent *pObj);
FRotator CalcShootRot(USTExtraShootWeaponComponent *pObj)
{
    if (qwcifqvs86y8fify)
    {
        qwcifqvs86y8fify = false;
        g_LocalController->bIsPressingFireBtn = false;
        pObj->OwnerShootWeapon->StopFire(EFreshWeaponStateType::FreshWeaponStateType_Idle);
    }

    if (Config.CalcShootRot.Enable)
    {
        ASTExtraPlayerCharacter *Target = GetTargetForAim();
        if (Target)
        {
            bool triggerOk = false;
            if (Config.AimMenu.Trigger != EAimTrigger::None)
            {
                if (Config.AimMenu.Trigger == EAimTrigger::Shooting)
                {
                    triggerOk = g_LocalPlayer->bIsWeaponFiring;
				
                }
                else if (Config.AimMenu.Trigger == EAimTrigger::Scoping)
                {
                    triggerOk = g_LocalPlayer->bIsGunADS;
                }
                else if (Config.AimMenu.Trigger == EAimTrigger::Both)
                {
                    triggerOk = g_LocalPlayer->bIsWeaponFiring && g_LocalPlayer->bIsGunADS;
                }
                else if (Config.AimMenu.Trigger == EAimTrigger::Any)
                {
                    triggerOk = g_LocalPlayer->bIsWeaponFiring || g_LocalPlayer->bIsGunADS;
                }
            }
            else
            triggerOk = true;
            if (triggerOk)
            {
                FVector targetAimPos = Target->GetBonePos("Head");
                if (Config.AimMenu.Target == EAimTarget::Head)
                {
                }
                else if (Config.AimMenu.Target == EAimTarget::Chest)
                {
                    targetAimPos.Z -= -09.0f;
                }
                UShootWeaponEntity *ShootWeaponEntityComponent = pObj->ShootWeaponEntityComponent;
                if (ShootWeaponEntityComponent)
                {
                    ASTExtraVehicleBase *CurrentVehicle = Target->CurrentVehicle;
                    if (CurrentVehicle)
                    {
                        FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;
                        float dist = g_LocalPlayer->GetDistanceTo(Target);
                        auto timeToTravel = dist / ShootWeaponEntityComponent->BulletFireSpeed;
                        targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(LinearVelocity, timeToTravel));
                    }
                    else
                    {
                        FVector Velocity = Target->GetVelocity();
                        float dist = g_LocalPlayer->GetDistanceTo(Target);
                        auto timeToTravel = dist / ShootWeaponEntityComponent->BulletFireSpeed;
                        targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(Velocity, timeToTravel));
                    }
                    FVector fDir = UKismetMathLibrary::Subtract_VectorVector(targetAimPos, g_LocalController->PlayerCameraManager->CameraCache.POV.Location);
                    return UKismetMathLibrary::Conv_VectorToRotator(fDir);
                }
            }
        }
    }
    return oCalcShootRot(pObj);
}
//==================================================================//
#define IM_PI 3.14159265358979323846f
#define RAD2DEG(x) ((float)(x) * (float)(180.f / IM_PI))
#define DEG2RAD(x) ((float)(x) * (float)(IM_PI / 180.f))

void VectorAnglesRadar(Vector3 & forward, FVector & angles) {
    if (forward.X == 0.f && forward.Y == 0.f) {
        angles.X = forward.Z > 0.f ? -90.f : 90.f;
        angles.Y = 0.f;
    } else {
        angles.X = RAD2DEG(atan2(-forward.Z, forward.Magnitude(forward)));
        angles.Y = RAD2DEG(atan2(forward.Y, forward.X));
    }
    angles.Z = 0.f;
}
void RotateTriangle(std::array<Vector3, 3> & points, float rotation) {
    const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
    for (auto & point : points) {
        point = point - points_center;
        const auto temp_x = point.X;
        const auto temp_y = point.Y;
        const auto theta = DEG2RAD(rotation);
        const auto c = cosf(theta);
        const auto s = sinf(theta);
        point.X = temp_x * c - temp_y * s;
        point.Y = temp_x * s + temp_y * c;
        point = point + points_center;
    }
}

FVector WorldToRadar(float Yaw, FVector Origin, FVector LocalOrigin, float PosX, float PosY, Vector3 Size, bool & outbuff) {
    bool flag = false;
    double num = (double)Yaw;
    double num2 = num * 0.017453292519943295;
    float num3 = (float)std::cosf(num2);
    float num4 = (float)std::sinf(num2);
    float num5 = Origin.X - LocalOrigin.X;
    float num6 = Origin.Y - LocalOrigin.Y;
    FVector Xector;
    Xector.X = (num6 * num3 - num5 * num4) / 150.f;
    Xector.Y = (num5 * num3 + num6 * num4) / 150.f;
    FVector Xector2;
    Xector2.X = Xector.X + PosX + Size.X / 2.f;
    Xector2.Y = -Xector.Y + PosY + Size.Y / 2.f;
    bool flag2 = Xector2.X > PosX + Size.X;
    if (flag2) {
        Xector2.X = PosX + Size.X;
    } else {
        bool flag3 = Xector2.X < PosX;
        if (flag3) {
            Xector2.X = PosX;
        }
    }
    bool flag4 = Xector2.Y > PosY + Size.Y;
    if (flag4) {
        Xector2.Y = PosY + Size.Y;
    } else {
        bool flag5 = Xector2.Y < PosY;
        if (flag5) {
            Xector2.Y = PosY;
        }
    }
    bool flag6 = Xector2.Y == PosY || Xector2.X == PosX;
    if (flag6) {
        flag = true;
    }
    outbuff = flag;
    return Xector2;
}

FVector2D pushToScreenBorder(FVector2D Pos, FVector2D screen, int borders, int offset) {
    int x = (int)Pos.X;
    int y = (int)Pos.Y;
    if ((borders & 1) == 1) {
        y = 0 - offset;
    }
    if ((borders & 2) == 2) {
        x = (int)screen.X + offset;
    }
    if ((borders & 4) == 4) {
        y = (int)screen.Y + offset;
    }
    if ((borders & 8) == 8) {
        x = 0 - offset;
    }
    return FVector2D(x, y);
}

int isOutsideSafezone(FVector2D pos, FVector2D screen) {
    FVector2D mSafezoneTopLeft(screen.X * 0.04f, screen.Y * 0.04f);
    FVector2D mSafezoneBottomRight(screen.X * 0.96f, screen.Y * 0.96f);
    int result = 0;
    if (pos.Y < mSafezoneTopLeft.Y) {
        result |= 1;
    }
    if (pos.X > mSafezoneBottomRight.X) {
        result |= 2;
    }
    if (pos.Y > mSafezoneBottomRight.Y) {
        result |= 4;
    }
    if (pos.X < mSafezoneTopLeft.X) {
        result |= 8;
    }
    return result;
}

void Box4Line(ImDrawList *draw, float thicc, int x, int y, int w, int h, int color) {
    int iw = w / 4;
    int ih = h / 4;
    // top
    draw->AddRect(ImVec2(x, y),ImVec2(x + iw, y), color, thicc);
    draw->AddRect(ImVec2(x + w - iw, y),ImVec2(x + w, y), color, thicc);
    draw->AddRect(ImVec2(x, y),ImVec2(x, y + ih), color, thicc);
    draw->AddRect(ImVec2(x + w - 1, y),ImVec2(x + w - 1, y + ih), color, thicc);;
    // bottom
    draw->AddRect(ImVec2(x, y + h),ImVec2(x + iw, y + h), color, thicc);
    draw->AddRect(ImVec2(x + w - iw, y + h),ImVec2(x + w, y + h), color, thicc);
    draw->AddRect(ImVec2(x, y + h - ih), ImVec2(x, y + h), color, thicc);
    draw->AddRect(ImVec2(x + w - 1, y + h - ih), ImVec2(x + w - 1, y + h), color, thicc);
}

void DrawBoxEnemy(ImDrawList *draw, ImVec2 X, ImVec2 Y, float thicc, int color) {
    draw->AddLine({X.x, X.y}, {Y.x, Y.y}, color, thicc);
}
float (*killer_3503040)(unsigned long long a1, int a2, float a3);
float brosorry012_3503040(unsigned long long a1, int a2, float a3) {
    return fmaxf(a3, 0.3f);
}
bool UnlimitedAmmo;

void RenderESPPRIVATE(AHUD *HUD, int ScreenWidth, int ScreenHeight){

if (UnlimitedAmmo && g_LocalPlayer && g_LocalPlayer->WeaponManagerComponent && g_LocalController) 
{
    if (g_LocalController->BackpackComponent) 
    {
        auto currentWeapon = (SDK::ASTExtraShootWeapon*)g_LocalPlayer->WeaponManagerComponent->CurrentWeaponReplicated;
        if (currentWeapon && currentWeapon->ShootWeaponEntityComp) 
        {
            static auto lastUseTime = std::chrono::steady_clock::now();
            auto currentTime = std::chrono::steady_clock::now();
            
            if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUseTime).count() > 600)
            {
                SDK::UBackpackUtils* utils = (SDK::UBackpackUtils*)SDK::UBackpackUtils::StaticClass();
                if (utils) 
                {
                    try {
                        
                        SDK::TArray<SDK::FBattleItemData> items = utils->GetAllItemsInBackpack(g_LocalController->BackpackComponent, 1);
                         
                        for (int i = 0; i < items.Num(); i++) 
                        {
                            const SDK::FBattleItemData& item = items[i];
                            
                            
                            if (item.DefineID.TypeSpecificID == 204013 || item.DefineID.TypeSpecificID == 204011) 
                            {
                                if (g_LocalController && g_LocalController->BackpackComponent) 
                                {
                                  
                                    g_LocalController->ServerUseItem(
                                        item.DefineID, 
                                        SDK::FBattleItemUseTarget(), 
                                        (SDK::EBattleItemUseReason)0
                                    );
                                                                       
                                    lastUseTime = currentTime;
                                    break; 
                                }
                            }
                        }
                    } 
                    catch (...) {
                        
                    }
                }
            }
        }
    }
}
}
//===================== ESP DRAW =====================//
void DrawESP(ImDrawList *draw) {
    
std::string telegram = "RAMBO || AUTO MAG || POWER";
float fontSize = 35.0f;
auto telegramTextSize = ImGui::CalcTextSize2(telegram.c_str(), 0, fontSize);

float telegramPosX = (glWidth - telegramTextSize.x) / 2.0f;
float telegramPosY = 600.0f;

ImDrawList* dl = ImGui::GetForegroundDrawList();
float time = ImGui::GetTime() * 2.0f; // speed control

// Tiranga Colors
ImVec4 saffron = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);
ImVec4 white   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 green   = ImVec4(0.0f, 0.6f, 0.2f, 1.0f);

// Outline config
ImU32 outlineCol = IM_COL32(0, 0, 0, 255);
float outlineOffset = 1.0f;
ImVec2 offsets[8] = {
    ImVec2(-outlineOffset, 0),
    ImVec2(outlineOffset, 0),
    ImVec2(0, -outlineOffset),
    ImVec2(0, outlineOffset),
    ImVec2(-outlineOffset, -outlineOffset),
    ImVec2(outlineOffset, -outlineOffset),
    ImVec2(-outlineOffset, outlineOffset),
    ImVec2(outlineOffset, outlineOffset),
};

// Glow config (soft shadow)
ImU32 glowCol = IM_COL32(0, 0, 0, 100); // semi-transparent black
float glowRadius = 4.0f;                // glow spread
int glowSteps = 12;                     // smoothness

// Animation: cycle saffron -> white -> green
for (int i = 0; i < telegram.size(); i++) {
    float t = fmodf((i * 0.3f) + time, 3.0f);
    ImVec4 colVec;

    if (t < 1.0f) colVec = saffron;
    else if (t < 2.0f) colVec = white;
    else colVec = green;

    ImU32 col = IM_COL32(
        (int)(colVec.x * 255),
        (int)(colVec.y * 255),
        (int)(colVec.z * 255),
        (int)(colVec.w * 255));

    // Each character size
    std::string ch(1, telegram[i]);
    auto charSize = ImGui::CalcTextSize2(ch.c_str(), 0, fontSize);

    // --- Glow pass (circular around text)
    for (int g = 0; g < glowSteps; g++) {
        float angle = (2 * IM_PI / glowSteps) * g;
        float dx = cosf(angle) * glowRadius;
        float dy = sinf(angle) * glowRadius;
        dl->AddText(nullptr, fontSize,
            ImVec2(telegramPosX + dx, telegramPosY + dy),
            glowCol, ch.c_str());
    }

    // --- Outline pass (8 directions, solid black)
    for (int j = 0; j < 8; j++) {
        dl->AddText(nullptr, fontSize,
            ImVec2(telegramPosX + offsets[j].x, telegramPosY + offsets[j].y),
            outlineCol, ch.c_str());
    }

    // --- Main colored text
    dl->AddText(nullptr, fontSize, ImVec2(telegramPosX, telegramPosY), col, ch.c_str());

    telegramPosX += charSize.x; // next char position
}
   /// ==================== FPS ==========

static ImVec2 fpsPos = ImVec2(10.0f, 10.0f);   // Initial position
static float fpsSize = 40.0f;                  // Font size
static ImVec2 fpsBoxSize = ImVec2(120, 50);    // Box size for padding
static float smoothFPS = 0.0f;
static float bgAlpha = 0.4f;                   // Background transparency
static float outlineThickness = 2.0f;          // Outline thickness
static bool isDragging = false;                // Track drag state
ImVec2 screenSize = ImGui::GetIO().DisplaySize;
float fps = ImGui::GetIO().Framerate;
smoothFPS = smoothFPS * 0.9f + fps * 0.1f;
char RLFLYPOWER[32];
snprintf(RLFLYPOWER, sizeof(RLFLYPOWER), OBFUSCATE(" %.2f"), smoothFPS);
ImColor fpsColor;
if (smoothFPS >= 50.0f) {
    fpsColor = ImColor(0, 255, 0, 255);
} else if (smoothFPS >= 30.0f) {
    fpsColor = ImColor(255, 255, 0, 255);
} else {
    fpsColor = ImColor(255, 0, 0, 255);
}
if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if ((mousePos.x >= fpsPos.x && mousePos.x <= fpsPos.x + fpsBoxSize.x) &&
        (mousePos.y >= fpsPos.y && mousePos.y <= fpsPos.y + fpsBoxSize.y)) {
        isDragging = true;
    }
    if (isDragging) {
        fpsPos.x += ImGui::GetIO().MouseDelta.x;
        fpsPos.y += ImGui::GetIO().MouseDelta.y;
        if (fpsPos.x < 0) fpsPos.x = 0;
        if (fpsPos.y < 0) fpsPos.y = 0;
        if (fpsPos.x + fpsBoxSize.x > screenSize.x) fpsPos.x = screenSize.x - fpsBoxSize.x;
        if (fpsPos.y + fpsBoxSize.y > screenSize.y) fpsPos.y = screenSize.y - fpsBoxSize.y;
    }
} else {
    isDragging = false;
}
draw->AddRectFilled(fpsPos, ImVec2(fpsPos.x + fpsBoxSize.x, fpsPos.y + fpsBoxSize.y), ImColor(0, 0, 0, (int)(bgAlpha * 255)));
draw->AddRect(fpsPos, ImVec2(fpsPos.x + fpsBoxSize.x, fpsPos.y + fpsBoxSize.y), ImColor(255, 255, 255, 255), 0.0f, 0, outlineThickness);
draw->AddText(NULL, fpsSize, ImVec2(fpsPos.x + 5, fpsPos.y + 5), fpsColor, RLFLYPOWER);



    
if(Config.ESPMenu.Active){
Config.ESPMenu.Line = true;
Config.ESPMenu.Skeleton = true;
Config.ESPMenu.Name = true;
Config.ESPMenu.Health = true;
Config.ESPMenu.Distance = true;
Config.ESPMenu.Vehicle = true;
Config.ESPMenu.LootBox = true;
Config.AimMenu.VisCheck = true;
Config.ESPMenu.EnemyWeapon =true;
Config.HighRisk.Recoil = true;
Config.HighRisk.Shake = true ;
Config.HighRisk.HitEffect = true;
Config.CalcShootRot.Enable = true ;
Config.HighRisk.WideView = true ;
}else{
Config.CalcShootRot.Enable = false ;
Config.HighRisk.Parachute = false;
Config.HighRisk.WideView = false;
Config.HighRisk.UnlimitedAmmo1 = false;
UnlimitedAmmo = false;
Config.HighRisk.UnlimitedAmmo = false;
Config.HighRisk.HitEffect = false;
Config.HighRisk.Shake = false ;
Config.HighRisk.Recoil = false;
Config.ESPMenu.EnemyWeapon =false;
Config.ESPMenu.Line = false;
Config.ESPMenu.Skeleton = false;
Config.ESPMenu.Name = false;
Config.ESPMenu.Health = false;
Config.ESPMenu.Distance = false;
Config.ESPMenu.Vehicle = false;
Config.ESPMenu.LootBox = false;
Config.AimMenu.VisCheck = false;
}
       if (Config.HighRisk.Flash) {
        Patches.Flash01.Modify();
        Patches.Flash02.Modify();
        Patches.Flash03.Modify();
        Patches.Flash04.Modify();
        Patches.Flash05.Modify();
    } else {
        Patches.Flash01.Restore();
        Patches.Flash02.Restore();
        Patches.Flash03.Restore();
        Patches.Flash04.Restore();
        Patches.Flash05.Restore();
    }
 
	
	
	
if(Config.ESPMenu.Active){
MemoryPatch::createWithHex("libanogs.so", 0x127468, "00 00 80 D2 C0 03 5F D6",32).Modify();
MemoryPatch::createWithHex("libanogs.so", 0x10D348, "00 00 80 D2 C0 03 5F D6",32).Modify();
MemoryPatch::createWithHex("libanogs.so", 0xc5ba0, "00 00 80 D2 C0 03 5F D6",32).Modify();

        }
  
for (auto &i: items_data) {
int itemCount = 0;
for (auto &item: i[("Items")]) {
item[("itemName")].get<std::string>().c_str();
Items[item[("itemId")].get<int>()] = true;
itemCount++;
if (itemCount % 4 != 0 && &item != &i[("Items")].back()) {
}
}
}
    
        if (Config.ESPMenu.Active){
    auto Actors = getActors();

            int totalEnemies = 0, totalBots = 0;
	ASTExtraPlayerCharacter *localPlayer = 0;
	ASTExtraPlayerController *localController = 0;
	ASTExtraShootWeapon *localWeapon = 0;

            for (int i = 0; i < Actors.Num(); i++)
            {
                auto Actor = Actors[i];
                if (isObjectInvalid(Actor))
                continue;

                if (Actor->IsA(ASTExtraPlayerController::StaticClass()))
                {
                    localController = (ASTExtraPlayerController *)Actor;
                    break;
                }
            }

           
    if (localController)
    {
        for (int i = 0; i < Actors.Num(); i++)
        {
            auto Actor = Actors[i];
            if (isObjectInvalid(Actor))
                continue;

            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass()))
            {
                if (((ASTExtraPlayerCharacter *)Actor)->PlayerKey == localController->PlayerKey)
                {
                    localPlayer = (ASTExtraPlayerCharacter *)Actor;
                    break;
                }
            }
        }
        
        
        
			

        

    if (localPlayer){
  
    auto GWorld = *(UWorld **)(UE4 + 0x70BD278);
    if (Config.HighRisk.Flash1) {
        g_LocalPlayer->StandHalfHeight = 77.0f;
        g_LocalPlayer->CrouchHalfHeight = -63.0f;
        GWorld->PersistentLevel->WorldSettings->MinUndilatedFrameTime = 0.100f;
        Tools::WriteAddr((void *) (UE4 + 0x3A662A0), (void *) "\x00\x00\x00\x00\x00\x00\x10\x41", 8);//gamespeed
        Tools::WriteAddr((void *) (UE4 + 0x3A66394), (void *) "\x1f\x20\x03\xd5\x4a\xf8\xff\x5c", 8);//gamespeed
        Tools::WriteAddr((void *) (UE4 + 0x3B56C30), (void *) "\xe3\x03\x27\x1e", 4);//fixstuck
        Tools::WriteAddr((void *) (UE4 + 0x3B56C2C), (void *) "\x17\x00\x80\x52", 4);//fixstuck
        Tools::WriteAddr((void *) (UE4 + 0x34E85F0), (void *) "\x17\x00\x80\x52", 4);//slowmotion
        HOOK((UE4 + 0x3503040), brosorry012_3503040, &killer_3503040);

        
    }
	if (localPlayer){
    if (Config.AimMenu.AutoFire) {
                if (GetTargetForAim()) {
                    localController->bIsPressingFireBtn = true;
                } else {
                    qwcifqvs86y8fify = true;
                }
           }}}
	
	      
	            if (localPlayer) {
            
            auto CharacterMovement = *(uintptr_t *)(uintptr_t(localPlayer) + 0x3f8);
            if (CharacterMovement) {
                if (Config.HighRisk.SkyFly) {
                    *(float*)(CharacterMovement + 0x22c) = 99999.9f;
                    *(float*)(CharacterMovement + 0x1ac) = 5.0f;
                    *(float*)(CharacterMovement + 0x1b4) = 8000.0f;
                    *(float*)(CharacterMovement + 0x208) = 999999.9f;
                } else {
                    *(float*)(CharacterMovement + 0x22c) = 0.05f;
                    *(float*)(CharacterMovement + 0x1ac) = 1.0f;
                    *(float*)(CharacterMovement + 0x1b4) = 443.0f;
                    *(float*)(CharacterMovement + 0x208) = 8192.9f;
                }
            }
            
            if (Config.HighRisk.SkyFly) {
                *(float *)(uintptr_t(localPlayer) + 0x4c8) = 999999.0f;
                *(float *)(uintptr_t(localPlayer) + 0x1b6c) = 10.0f;
                *(float *)(uintptr_t(localPlayer) + 0x88) = 0.1f;
            } else {
                *(float *)(uintptr_t(localPlayer) + 0x4c8) = 1.0f;
                *(float *)(uintptr_t(localPlayer) + 0x1b6c) = 1.0f;
                *(float *)(uintptr_t(localPlayer) + 0x88) = 1.0f;
            }
                
            
	
    if (Config.HighRisk.Parachute){
    UCharacterParachuteComponent *ParachuteComponent = localPlayer->ParachuteComponent;
    if (ParachuteComponent) {
        ParachuteComponent->CurrentFallSpeed = 9999.9f;
        }
    }
    //===================MEMORY-FUNCTION====================//
    auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
    if (WeaponManagerComponent){
    auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)WeaponManagerComponent->CurrentWeaponReplicated;
    if (CurrentWeaponReplicated){
        
    if (Config.HighRisk.KillMsg){
    auto Records = localController->ClientFatalDamageRecords;
    for (int i = 0; i < Records.Num(); i++){
    if (Records[i].RecordRelationShip == EFatalDamageRelationShip::MyTeamateIsCauser){
                                        
                                            }}}
                               
    
    auto ShootWeaponEntityComp = CurrentWeaponReplicated->ShootWeaponEntityComp;
    auto ShootWeaponEffectComp = CurrentWeaponReplicated->ShootWeaponEffectComp;
    if (ShootWeaponEntityComp && ShootWeaponEffectComp){
        
    if (Config.HighRisk.Recoil){
    ShootWeaponEntityComp->AccessoriesVRecoilFactor = 0.01f;
    ShootWeaponEntityComp->AccessoriesHRecoilFactor = 0.01f;
    ShootWeaponEntityComp->GameDeviationFactor = 0.3f;
    ShootWeaponEntityComp->RecoilKickADS = 0.01f;
    }
    
    if (Config.HighRisk.Shake){
    ShootWeaponEntityComp->AnimationKick = 0.01f;
    }
    
    if (Config.HighRisk.HitEffect) {
    auto MyHUD = *(uintptr_t *)((uintptr_t)localController + 0x420);
    auto XHitAddr = *(uintptr_t *)((uintptr_t)MyHUD + 0x490);
    *(float *)((uintptr_t)XHitAddr + 0x10) = 9999.0f;
                *(float *)((uintptr_t)XHitAddr + 0x50) = 999.0f;
                *(float *)((uintptr_t)XHitAddr + 0x90) = 999.0f;
                *(float *)((uintptr_t)XHitAddr + 0xD0) = 999.0f;
           }
    
    if (Config.HighRisk.Instant){
    ShootWeaponEntityComp->ShootInterval = 0.1f;
    ShootWeaponEntityComp->BurstShootInterval = 0.1f;
    }
    
    // Removed invalid placeholder code: GameSession was explicitly null and
    // immediately dereferenced whenever ESPMenu.Active was enabled.
    
    }}}//WeaponManagerComponentEnd....
    if (FASTSHOOT) {
if (localPlayer->bIsWeaponFiring || localPlayer->bIsGunADS) {
auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
  if (WeaponManagerComponent) {
  auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();

  if ((int) propSlot.GetValue() >= 1 && (int) propSlot.GetValue() <= 3) {
  auto CurrentWeaponReplicated = (ASTExtraShootWeapon *) WeaponManagerComponent->CurrentWeaponReplicated;
  if (CurrentWeaponReplicated) {
  auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;

  if (ShootWeaponComponent) {
  UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
  if (ShootWeaponEntityComponent) {

auto STExtraBaseCharacter = localController->STExtraBaseCharacter;

if (STExtraBaseCharacter != nullptr)
{
   auto CurrentVehicle = STExtraBaseCharacter->CurrentVehicle;
   
   if (CurrentVehicle == nullptr)
   {
if (FASTSHOOT) {
ShootWeaponEntityComponent->ShootInterval = ControlSpeed;
}}}}}}}}}}
                //===================AIMBOT====================//
                 if (Config.AimBot.Enable) {
                    ASTExtraPlayerCharacter *Target = GetTargetForAim();
                    if (Target) {
                        bool triggerOk = false;
                        if (Config.AimMenu.Trigger != EAimTrigger::None)
                        {
                            if (Config.AimMenu.Trigger == EAimTrigger::Shooting)
                            {
                                triggerOk = localPlayer->bIsWeaponFiring;
                            }
                            else if (Config.AimMenu.Trigger == EAimTrigger::Scoping)
                            {
                                triggerOk = localPlayer->bIsGunADS;
                            }
                            else if (Config.AimMenu.Trigger == EAimTrigger::Both)
                            {
                                triggerOk = localPlayer->bIsWeaponFiring && localPlayer->bIsGunADS;
                            }
                            else if (Config.AimMenu.Trigger == EAimTrigger::Any)
                            {
                                triggerOk = localPlayer->bIsWeaponFiring || localPlayer->bIsGunADS;
                            }
                        } else triggerOk = true;
                        if (triggerOk) {
                            FVector targetAimPos = Target->GetBonePos("Head");
                            if (Config.AimMenu.Target == EAimTarget::Head) {
                            } else if (Config.AimMenu.Target == EAimTarget::Chest) {
                                targetAimPos.Z -= 25.0f;
                            }
                            auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
                            if (WeaponManagerComponent)
                            {
                                auto propSlot = WeaponManagerComponent->GetCurrentUsingPropSlot();
                                if ((int)propSlot.GetValue() >= 1 && (int)propSlot.GetValue() <= 3)
                                {
                                    auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)WeaponManagerComponent->CurrentWeaponReplicated;
                                    if (CurrentWeaponReplicated)
                                    {
                                        auto ShootWeaponComponent = CurrentWeaponReplicated->ShootWeaponComponent;
                                        if (ShootWeaponComponent)
                                        {
                                            UShootWeaponEntity *ShootWeaponEntityComponent = ShootWeaponComponent->ShootWeaponEntityComponent;
                                            if (ShootWeaponEntityComponent)
                                            {
                                                ASTExtraVehicleBase *CurrentVehicle = Target->CurrentVehicle;
                                                if (CurrentVehicle)
                                                {
                                                    FVector LinearVelocity = CurrentVehicle->ReplicatedMovement.LinearVelocity;
                                                    float dist = localPlayer->GetDistanceTo(Target);
                                                    auto timeToTravel = dist / ShootWeaponEntityComponent->BulletFireSpeed;
                                                    targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(LinearVelocity, timeToTravel));
                                                } else {
                                                    FVector Velocity = Target->GetVelocity();
                                                    float dist = localPlayer->GetDistanceTo(Target);
                                                    auto timeToTravel = dist / ShootWeaponEntityComponent->BulletFireSpeed;
                                                    targetAimPos = UKismetMathLibrary::Add_VectorVector(targetAimPos, UKismetMathLibrary::Multiply_VectorFloat(Velocity, timeToTravel));
                                                }
                                                localController->SetControlRotation(ToRotator(localController->PlayerCameraManager->CameraCache.POV.Location, targetAimPos),"");
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                //Radar Background Simulation
                
                //===================ESP-VISUAL====================//
                for (int i = 0; i < Actors.Num(); i++) {
                     auto Actor = Actors[i];
                     if (isObjectInvalid(Actor))
                         continue;
                         
                            if (Actor->IsA(ASTExtraPlayerCharacter::StaticClass())) {
                            long PlayerLineClrCf = IM_COL32(0, 255, 153, 225);//VISIBLE BOT HEALTH 
                            long PlayerBoxClrCf = IM_COL32(255, 252, 0, 255);//VISIBLE BOX & HEALTH BORDER & LINE PLAYER
                            long PlayerBoxClrCf2 = IM_COL32(102, 255, 51, 255);//VISIBLE BOX & LINE
                            long PlayerBoxClrCf3 = IM_COL32(255, 153, 0, 255);//VISIBLE BOX & LINE
                            long PlayerSkeltonClrCf = IM_COL32(255, 255, 0, 255);//VISIBLE SKELETON 
                          long  SCOLOR = IM_COL32(0, 204, 255, 255);//VISIBLE BOT HEALTH BORDER 
                        long    SCOLO = IM_COL32(255, 0, 0, 140);//VISIBLE HEALTH BAR PLAYER 
                        
                            auto Player = (ASTExtraPlayerCharacter *) Actor;
                            if (!localController->LineOfSightTo(Player, {0, 0, 0}, true)){
                                PlayerLineClrCf = IM_COL32(255, 255, 255, 140);//NOT VISIBLE BOT HEALTH
                                PlayerBoxClrCf = IM_COL32(255, 0, 0, 255);//NOT VISIBLE BOX & LINE & HEALTH BORDER PLAYER 
                                PlayerBoxClrCf2 = IM_COL32(255, 255, 255, 255);//NOT VISIBLE BOT BOX & LINE 
                                PlayerBoxClrCf3 = IM_COL32(0, 153, 255, 255);//NOT VISIBLE BOX & LINE
                                PlayerSkeltonClrCf = IM_COL32(255, 255, 255, 255);//NOT VISIBLE SKELETON 
                                SCOLOR = IM_COL32(153, 51, 255, 255);//NOT VISIBLE BOT HEALTH BORDER 
                               SCOLO = IM_COL32(51, 204, 204, 140);//NOT VISIBLE HEALTH BAR PLAYER 
                            }
                            
                            float Distance = localPlayer->GetDistanceTo(Player) / 100.0f;
                            if (Distance > 500.0f)
                                continue;
                            
                            if (Player->PlayerKey == localController->PlayerKey)
                                continue;
                            
                            if (Player->TeamID == localController->TeamID)
                                continue;
                            
                            if (Player->bDead)
                                continue;
                                
                            if (Player->bHidden)
                                continue;
                                
                            if (Config.AimMenu.Type == EAimType::Croshhair) {
                                draw->AddCircle(ImVec2(glWidth / 2, glHeight / 2), Config.AimMenu.FOVSize, IM_COL32(0, 255, 0, 255), 100, 0.8f);
                            }

                            
long PBox, PLine, PSkeleton, SOLO;
if (Player->bIsAI) {
totalBots++;
PBox = IM_COL32(255, 255, 255, 255);

SOLO = IM_COL32(0, 0, 0, 255);
if (!localController->LineOfSightTo(Player, {0, 0, 0}, true)) {
PBox = IM_COL32(255, 255, 255, 255);

SOLO = IM_COL32(0, 0, 0, 255);
}
} else {
totalEnemies++;
PBox = IM_COL32(255, 252, 0, 255);

SOLO = IM_COL32(255, 255, 255, 255);
if (!localController->LineOfSightTo(Player, {0, 0, 0}, true)) {
SOLO = IM_COL32(255, 255, 255, 255);
    
PBox = IM_COL32(255, 0, 0, 255);

}
}
                            
                    
                            float magic_number = (Distance);
                            float mx = (glWidth / 2) / magic_number;
                            float healthLength = glWidth / 20;
                            if (healthLength < mx)
                                healthLength < mx;
                                
                            
                            auto HeadPos = Player->GetBonePos("Head");
                            HeadPos.Z = HeadPos.Z + 1.0f; 
                            ImVec2 HeadPosSC;

                            auto RootPos = Player->GetBonePos("Root");
                            ImVec2 RootPosSC;
                            if (W2S(HeadPos, (FVector2D *) &HeadPosSC) && W2S(RootPos, (FVector2D *) &RootPosSC)) {
                                
           
                                
                                
                               if (Config.ESPMenu.Line){
                                    if (Player->bIsAI){
                                 //   draw->AddLine({(float) glWidth /  2, 3.5 + 70}, HeadPosSC,PlayerBoxClrCf2, 0.8f);         
                                 draw->AddLine({(float) glWidth / 2, 0}, ImVec2(HeadPosSC.x, HeadPosSC.y - 40.0f), PBox,2.0f);
                                         } else {
                                //    draw->AddLine({(float) glWidth /  2, 3.5 + 70}, HeadPosSC, PlayerBoxClrCf, 0.8f);        
                                 draw->AddLine({(float) glWidth / 2, 0}, ImVec2(HeadPosSC.x, HeadPosSC.y - 40.0f), PBox,2.0f);
                                     }
                                     }
                                     
                                     
                 
                            
                            if (Config.ESPMenu.Box) {
                            float boxHeight = abs(HeadPosSC.y - RootPosSC.y);
                            float boxWidth = boxHeight * 0.65f;
                            ImVec2 vStart = {HeadPosSC.x - (boxWidth / 2), HeadPosSC.y};
                            ImVec2 vEnd = {vStart.x + boxWidth, vStart.y + boxHeight};
                            draw->AddRect(vStart, vEnd, PBox, 2.5f, 240, 1.7f);
                        }
                                    

                                if (Config.ESPMenu.Skeleton) {                                    
                                    static std::vector<std::string> right_arm{"neck_01",
                                                                              "clavicle_r",
                                                                              "upperarm_r",
                                                                              "lowerarm_r",
                                                                              "hand_r", "item_r"};
                                    static std::vector<std::string> left_arm{"neck_01",
                                                                             "clavicle_l",
                                                                             "upperarm_l",
                                                                             "lowerarm_l",
                                                                             "hand_l", "item_l"};
                                    static std::vector<std::string> spine{"Head", "neck_01",
                                                                          "spine_03",
                                                                          "spine_02", "spine_01",
                                                                          "pelvis"};
                                    static std::vector<std::string> lower_right{"pelvis", "thigh_r",
                                                                                "calf_r", "foot_r"};
                                    static std::vector<std::string> lower_left{"pelvis", "thigh_l",
                                                                               "calf_l", "foot_l"};
                                    static std::vector<std::vector<std::string>> skeleton{right_arm,
                                                                                          left_arm,
                                                                                          spine,
                                                                                          lower_right,
                                                                                          lower_left};

                                    for (auto &boneStructure: skeleton) {
                                        std::string lastBone;
                                        for (std::string &currentBone: boneStructure) {
                                            if (!lastBone.empty()) {
                                                ImVec2 boneFrom, boneTo;
                                                if (W2S(Player->GetBonePos(lastBone.c_str()),
                                                        (FVector2D *) &boneFrom) &&
                                                    W2S(Player->GetBonePos(currentBone.c_str()),
                                                        (FVector2D *) &boneTo)) {
                                                    draw->AddLine(boneFrom, boneTo,
                                                                  PBox, 1.3f);
                                                }
                                            }
                                            lastBone = currentBone;
                                        }
                                    }
                                }           
								
								
                               if (Config.ESPMenu.Health) {
                                            int CurHP = (int) std::max(0, std::min(
                                                        (int) Player->Health,
                                                        (int) Player->HealthMax));
                                                int MaxHP = (int) Player->HealthMax;

                                            long HPColor = IM_COL32(std::min(((510 * (MaxHP - CurHP)) / MaxHP), 255), std::min((510 * CurHP) / MaxHP, 255), 0, 150);
                                                        
                                                /*  long HPColor;
                                                
                                                       HPColor = SCOLOR;*/
                                                if (Player->Health == 0.0f && !Player->bDead) {
                                                    
                                                    
                                                    
                                                   
                                                  //  CurHP = Player->NearDeathBreath;
                                                    if (Player->NearDeatchComponent) {
                                                        MaxHP = Player->NearDeatchComponent->BreathMax;
                                                    }
                                                }

                                                if (Player->bIsAI) {
                                                    
                                                //SCOLOR = SCOLOR;
                                                HPColor = PBox;
                                              //  long HPColor = IM_COL32(std::min(((510 * (MaxHP - CurHP)) / MaxHP), 255), std::min((510 * CurHP) / MaxHP, 255), 0, 150);
                                                        
                                                

                                                float boxWidth = density / 1.8f;
                                                boxWidth -= std::min(
                                                        ((boxWidth / 2) / 0.003f) * Distance,
                                                        boxWidth / 2);
                                                float boxHeight = boxWidth * 0.17f;


                                                ImVec2 vStart = {HeadPosSC.x - (boxWidth / 2),
                                                                 HeadPosSC.y -
                                                                 (boxHeight * 2.001f)};

                                                
                                                ImVec2 vEndFilled = {
                                                        vStart.x + (CurHP * boxWidth / MaxHP),
                                                        vStart.y + boxHeight};
                                                ImVec2 vEndRect = {vStart.x + boxWidth,
                                                                   vStart.y + boxHeight};

                                                draw->AddRectFilled(vStart, vEndFilled, HPColor,
                                                                    10.0f);
                                                draw->AddRect(vStart, vEndRect,
                                                              IM_COL32(0, 0, 0, 155), 10.0f);
                                                              
                                                                } else {
                                                                    
                                            /*  SCOLOR = PlayerBoxClrCf;
                                               HPColor = SCOLO;*/
                                               
                                               long HPColor = IM_COL32(std::min(((510 * (MaxHP - CurHP)) / MaxHP), 255), std::min((510 * CurHP) / MaxHP, 255), 0, 150);
                                                        

                                                float boxWidth = density / 1.8f;
                                                boxWidth -= std::min(
                                                        ((boxWidth / 2) / 0.003f) * Distance,
                                                        boxWidth / 2);
                                                float boxHeight = boxWidth * 0.17f;


                                                ImVec2 vStart = {HeadPosSC.x - (boxWidth / 2),
                                                                 HeadPosSC.y -
                                                                 (boxHeight * 2.001f)};

                                                
                                                ImVec2 vEndFilled = {
                                                        vStart.x + (CurHP * boxWidth / MaxHP),
                                                        vStart.y + boxHeight};
                                                ImVec2 vEndRect = {vStart.x + boxWidth,
                                                                   vStart.y + boxHeight};

                                                draw->AddRectFilled(vStart, vEndFilled, HPColor,
                                                                    10.0f);
                                                draw->AddRect(vStart, vEndRect,
                                                              IM_COL32(0, 0, 0, 155), 10.0f);

                                                              }
                                                                    
                                }
                                
                          if (Config.ESPMenu.Name) {
                                    float boxWidth = density / 2.0f;
                                               boxWidth -= std::min(
                                               ((boxWidth / 2) / 00.0f) * Distance,
                                               boxWidth / 2);
                                               float boxHeight = boxWidth * 0.19f;
                                               std::string s;
                                               if (Player->bIsAI) {
                                               s += "        BOT";
                                               } else {
                                               s += Player->PlayerName.ToString();
                                               }
                                               draw->AddText(NULL, ((float) density / 30.0f),
                                               {HeadPosSC.x - (boxWidth / 3.3),
                                               HeadPosSC.y - (boxHeight * 1.83f)},
                                               SOLO,
                                               s.c_str());
                                
                                    
                                }
                                
                            
                                if (Config.ESPMenu.Distance) {
                                    
                                    
                             std::string s = std::to_string((int) Distance) + "M";
                            auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, ((float) density / 22.0f));
                            ImVec2 textPos(RootPosSC.x - (textSize.x / 2.0f), RootPosSC.y);
                            draw->AddText(NULL, ((float) density / 22.0f), textPos, IM_COL32(255, 165, 0, 255), s.c_str());
                                    
                                }

                                
                            
                            if (Config.ESPMenu.EnemyWeapon) {
                                auto WeaponManagerComponent = Player->WeaponManagerComponent;
                                if (WeaponManagerComponent) {
                                        auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)WeaponManagerComponent->CurrentWeaponReplicated;
                                        if (CurrentWeaponReplicated) {
                                            auto WeaponId = (int)CurrentWeaponReplicated->GetWeaponID();
                                            if (WeaponId) {
                                                std::string s;
                                                s += CurrentWeaponReplicated->GetWeaponName().ToString();
                                              //  s += "                  ";
                                             //   s += std::to_string((int) CurrentWeaponReplicated->CurBulletNumInClip);         
                                                
                                                //auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, ((float) density / 20.0f));
                                                draw->AddText(NULL, ((float) density / 23.0f), {HeadPosSC.x - 65.0f , HeadPosSC.y - 65.0f}, IM_COL32(255, 252, 0, 255), s.c_str());
          
                                                
                                                
                                              /*  auto textSize = ImGui::CalcTextSize2(s.c_str(), 0, ((float) density / 35.0f));
                                                draw->AddText(NULL, ((float) density / 30.0f), {HeadPosSC.x - (textSize.x / 2), HeadPosSC.y}, IM_COL32(255, 165, 0, 255), s.c_str());*/
                                            }
                                        }
                                    }
                                }
                            } //WS2 End...
                            
                            //360° ESP DRAW Circle
                            FVector2D screen(glWidth, glHeight);
                            FVector2D location(RootPosSC.x, HeadPosSC.y);
                            int borders = isOutsideSafezone(location, screen);
                            if (Config.ESPMenu.Alert && borders != 0) 
                                {
                                    float Distance = localPlayer->GetDistanceTo(Player) / 100.0f;
                                    std::string s;
                                    s += std::to_string((int)Distance);
                                    s += "M";
                                    float mScale = glHeight / (float) 1080;
                                    auto textSize = ImGui::CalcTextSize(s.c_str(), 0, s.size());
                                    auto hintDotRenderPos = pushToScreenBorder(location, screen, borders, (int)((mScale * 100) / 3));
                                    auto hintTextRenderPos = pushToScreenBorder(location, screen, borders, -(int)((mScale * 36)));
                                    draw->AddCircleFilled(ImVec2(hintDotRenderPos.X, hintDotRenderPos.Y), mScale * 100, IM_COL32(255, 0, 0, 128), 0);
                                    draw->AddText(NULL, ((float)density / 30.0f), ImVec2(hintTextRenderPos.X, hintTextRenderPos.Y), IM_COL32(255, 255, 255, 255), s.c_str());
                                }
                                
        
                        	if (Config.ESPMenu.GameInfo)
                    {
                        if (Actors[i]->IsA(ASTExtraGameStateBase::StaticClass()))
                        {
                            auto InGame = (ASTExtraGameStateBase *)Actors[i];
                            std::string s;
                            if ((int)InGame->AliveTeamNum == 1)
                            {
                                std::vector<std::string> lines = {
                                    "<<Winner Winner Chicken Dinner >>",
									"<< VEER SINGH  >>",
                                     "<<   Restart Your Game >>"};

                                float textPosY = 420; 
                                for (const auto &line : lines)
                                {
                                    auto textSize = ImGui::CalcTextSize(line.c_str(), 0, line.size());
                                    float textPosX = (glWidth - textSize.x) / 2;
                                    draw->AddText(NULL, ((float)density / 12.0f), ImVec2(textPosX, textPosY), IM_COL32(255, 255, 0, 255), line.c_str());
                                    textPosY += textSize.y + 25; 
                                }
                            }
                        }
                    }
							
		
                            
                    if (Config.ESPMenu.Vehicle) {
                        if (Actors[i]->IsA(ASTExtraVehicleBase::StaticClass())) {
                            auto Vehicle = (ASTExtraVehicleBase *)Actors[i];
                            if (!Vehicle->Mesh)
                                continue;
                            int CurHP = (int) std::max(0, std::min((int) Vehicle->VehicleCommon->HP, (int) Vehicle->VehicleCommon->HPMax));
                            int MaxHP = (int) Vehicle->VehicleCommon->HPMax;
                            long VHP_Color = IM_COL32(std::min(((510 * (MaxHP - CurHP)) / MaxHP), 255), std::min(((510 * CurHP) / MaxHP), 255), 0, 155);
                            float Distance = Vehicle->GetDistanceTo(localPlayer) / 100.f;
                            if (Distance > 500.0f) continue;
                            FVector2D vehiclePos;
                            if (W2S(Vehicle->K2_GetActorLocation(), &vehiclePos))
                            {
                                auto mWidthScale = std::min(0.10f * Distance, 50.f);
                                auto mWidth = 85.0f - mWidthScale;
                                auto mHeight = mWidth * 0.07f;
                                std::string s = GetVehicleName(Vehicle);
                                s += " [";
                                s += std::to_string((int)Distance);
                                s += "m]";
                                auto textSize = ImGui::CalcTextSize(s.c_str(), 0, s.size());
                                draw->AddText(NULL, ((float)density / 28.0f), {vehiclePos.X - (mWidth / 2), vehiclePos.Y}, IM_COL32(255, 255, 0, 255), s.c_str());              
                                }
                        }
                    }
                    
                    if (Config.ESPMenu.LootBox) {
                        if (Actors[i]->IsA(APlayerTombBox::StaticClass())) {
                            auto APickUpListWrapperActor = (APlayerTombBox *) Actors[i];
                            auto RootComponent = APickUpListWrapperActor->RootComponent;
                            if (!RootComponent) continue;
                            float Distance = APickUpListWrapperActor->GetDistanceTo(localPlayer) / 100.0f;
                            if (Distance > 40.0f) continue;
                            FVector2D lootboxPos;
                            if (W2S(APickUpListWrapperActor->K2_GetActorLocation(), &lootboxPos)) {
                                std::string s = GetPickUpName(APickUpListWrapperActor);
                                s += " [";
                                s += std::to_string((int) Distance);
                                s += "m]";
                                auto textSize = ImGui::CalcTextSize(s.c_str(), 0, s.size());
                                draw->AddText(NULL, ((float) density / 30.0f),
                                              {lootboxPos.X, lootboxPos.Y},
                                              IM_COL32(000, 255, 000, 255), s.c_str());
                            }
                        }
                    }
                    
                        if (Config.LootI) {
                                            
                                            if (Actors[i]->IsA(APickUpListWrapperActor::StaticClass())) {
                                        auto Pick = (APickUpListWrapperActor *) Actors[i];
                                        if (!Pick->RootComponent)
                                            continue;
                                        auto PickUpDataList = (TArray<FPickUpItemData>)Pick->GetDataList();
                                        float Distance = Pick->GetDistanceTo(localPlayer) / 100.f;
                                        FVector2D PickUpListsPos;
                                        Vector3 origin, extends;
                                        if (W2S(Pick->K2_GetActorLocation(), & PickUpListsPos)) {
                            
                                            
                                                if (Distance <= 50.f) {
                                                    auto mWidthScale = std::min(0.1f * Distance, 50.f);//150
                                                    auto boxWidth = 75.f - mWidthScale;
                                                    auto boxHeight = boxWidth * 0.120f;
                                                    Rect PlayerRect(PickUpListsPos.X - (boxWidth / 2), PickUpListsPos.Y, boxWidth, boxHeight);
                                                    float posY = PickUpListsPos.Y - (PlayerRect.height * 2.0f); // 1.00f // 1.5f
                                                    for (int j = 0; j < PickUpDataList.Num(); j++) {
                                                        std::vector<std::string> s2;
                                                        std::string itm;
                                                        uint32_t tc = 0xFF000000;
                                                        for (auto & category : items_data,items_data,items_data,items_data,items_data,items_data,items_data,items_data,items_data) {
                                                            for (auto & item : category["Items"]) {
                                                                if (item["itemId"] == PickUpDataList[j].ID.TypeSpecificID) {
                                                                    tc = strtoul(item["itemTextColor"].get<std::string>().c_str(), 0, 16);
                                                                    itm += item["itemName"].get<std::string>();
                                                                    s2.push_back(itm);
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                        if (!s2.empty())  {
                                                            if (PickUpDataList[j].count > 1) {
                                                                s2.push_back(" * ");
                                                                s2.push_back(std::to_string(PickUpDataList[j].count));
                                                            }
                                                            std::string s3;
                                                            for (auto & s4 : s2)   {
                                                                s3 += s4;
                                                            }
                                                            draw->AddText(NULL, ((float) density / 22.0f), {PickUpListsPos.X, posY}, tc, s3.c_str());
                                                            posY -= PlayerRect.height * 2.00f; // 1.00f // 1.25f
                                                 }}}}}}}
                        if (Actors[i]->IsA(APickUpWrapperActor::StaticClass())) {
                            auto PickUp = (APickUpWrapperActor *) Actors[i];
                            if (Items[PickUp->DefineID.TypeSpecificID]) {
                                auto RootComponent = PickUp->RootComponent;
                                if (!RootComponent) continue;
                                float Distance = PickUp->GetDistanceTo(localPlayer) / 100.0f;
                                if (Distance > 30.0f) continue;
                                FVector2D itemPos;
                                if (W2S(PickUp->K2_GetActorLocation(), &itemPos)) {
                                    std::string s;
                                    uint32_t tc = 0xFF000000;
                                    for (auto &category : items_data) {
                                        for (auto &item : category["Items"]) {
                                            if (item["itemId"] == PickUp->DefineID.TypeSpecificID) {
                                                s = item["itemName"].get<std::string>();
                                                tc = strtoul(
                                                        item["itemTextColor"].get<std::string>().c_str(),
                                                        0, 16);
                                                break;
                                            }
                                        }
                                    }
                                    s += " - ";
                                    s += std::to_string((int) Distance);
                                    s += "M";
                                    auto textSize = ImGui::CalcTextSize(s.c_str(), 0, s.size());
                                    draw->AddText(NULL, ((float) density / 30.0f),
                                                  {itemPos.X, itemPos.Y}, IM_COL32(255, 255, 255, 255), s.c_str());
                                }
                            }
                        }
}
               }
            }

    g_LocalController = localController;
g_LocalPlayer = localPlayer;   
int TotalCount = totalEnemies;
std::string 安全;
std::string 真人;
std::string 人机;


        if (totalEnemies > 0 || totalBots > 0)
{
    // Text parts
    std::string sEnemies = std::to_string((int)totalEnemies);
    std::string sBots    = std::to_string((int)totalBots);
    std::string sBar     = " | ";

    float fontSize = ((float)density / 12.0f);

    // Size calc
    ImVec2 sizeEnemies = ImGui::CalcTextSize2(sEnemies.c_str(), 0, fontSize);
    ImVec2 sizeBar     = ImGui::CalcTextSize2(sBar.c_str(), 0, fontSize);
    ImVec2 sizeBots    = ImGui::CalcTextSize2(sBots.c_str(), 0, fontSize);

    float totalWidth = sizeEnemies.x + sizeBar.x + sizeBots.x;
    float posX = (glWidth / 2) - (totalWidth / 2.0f);
    float posY = 50; // halka niche

    // 🔴 Enemy Count (Red with outline & glow)
    ImVec2 posEnemy = ImVec2(posX, posY);
    draw->AddText(NULL, fontSize, ImVec2(posEnemy.x+1,posEnemy.y+1), IM_COL32(0,0,0,200), sEnemies.c_str()); // shadow
    for (int i=0;i<2;i++) draw->AddText(NULL,fontSize,posEnemy,IM_COL32(255,0,0,120-i*40),sEnemies.c_str()); // glow
    draw->AddText(NULL, fontSize, posEnemy, IM_COL32(255,0,0,255), sEnemies.c_str()); // main text
    posX += sizeEnemies.x;

    // ⚪ Separator (White simple)
    ImVec2 posBar = ImVec2(posX, posY);
    draw->AddText(NULL, fontSize, ImVec2(posBar.x+1,posBar.y+1), IM_COL32(0,0,0,200), sBar.c_str()); 
    draw->AddText(NULL, fontSize, posBar, IM_COL32(255,255,255,255), sBar.c_str());
    posX += sizeBar.x;

    // 🟢 Bot Count (Lime with outline & glow)
    ImVec2 posBot = ImVec2(posX, posY);
    draw->AddText(NULL, fontSize, ImVec2(posBot.x+1,posBot.y+1), IM_COL32(0,0,0,200), sBots.c_str()); // shadow
    for (int i=0;i<2;i++) draw->AddText(NULL,fontSize,posBot,IM_COL32(0,255,128,120-i*40),sBots.c_str()); // glow
    draw->AddText(NULL, fontSize, posBot, IM_COL32(0,255,128,255), sBots.c_str()); // main text
}
else
{
    std::string s = "    CLEAR";
    float fontSize = ((float)density / 15.0f);
    ImVec2 textSize = ImGui::CalcTextSize2(s.c_str(), 0, fontSize);
    ImVec2 textPos  = ImVec2(glWidth / 2 - textSize.x / 2.0f, 50);

    // Shadow
    draw->AddText(NULL, fontSize, ImVec2(textPos.x+1,textPos.y+1), IM_COL32(0,0,0,200), s.c_str());
    // Glow
    for (int i=0;i<2;i++) draw->AddText(NULL,fontSize,textPos,IM_COL32(0,255,0,120-i*40),s.c_str());
    // Main
    draw->AddText(NULL, fontSize, textPos, IM_COL32(0,255,0,255), s.c_str());
}}}



Vector3 (*oCalcShootTargetLocByStartAndRot)(void *pObj, Vector3 Start, FRotator ShootRot, void *OutHitResult) = nullptr;

Vector3 CalcShootTargetLocByStartAndRot(void *pObj, Vector3 Start, FRotator ShootRot, void *OutHitResult)
{
    if (g_LocalPlayer && g_LocalController)
    {
        auto WeaponManager = g_LocalPlayer->WeaponManagerComponent;
        if (WeaponManager && WeaponManager->CurrentWeaponReplicated)
        {
            auto Target = GetTargetByDistance();
            if (Target && (g_LocalPlayer->bIsGunADS || g_LocalPlayer->bIsWeaponFiring))
            {

                SDK::FVector targetBone = Target->GetBonePos("Head");

                SDK::FVector startPos;
                startPos.X = Start.X;
                startPos.Y = Start.Y;
                startPos.Z = Start.Z;

                SDK::FVector direction;
                direction.X = targetBone.X - startPos.X;
                direction.Y = targetBone.Y - startPos.Y;
                direction.Z = targetBone.Z - startPos.Z;

                ShootRot = SDK::USTExtraBlueprintFunctionLibrary::Vector2Rotator(direction);
            }
        }
    }

    return oCalcShootTargetLocByStartAndRot(pObj, Start, ShootRot, OutHitResult);
}


void DrawTextCentered(const char *text)
{
    ImGui::Separator();
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(text).x) / 2.f);
    ImGui::Text(text);
    ImGui::Separator();
}

int OpenURL(const char* url)
{ 
    JavaVM* java_vm = g_App->activity->vm;
    JNIEnv* java_env = NULL; 
    jint jni_return = java_vm->GetEnv((void**)&java_env, JNI_VERSION_1_6);
    if (jni_return == JNI_ERR)
        return -1;
    jni_return = java_vm->AttachCurrentThread(&java_env, NULL);
    if (jni_return != JNI_OK) 
        return -2;
    jclass native_activity_clazz = java_env->GetObjectClass(g_App->activity->clazz);
    if (native_activity_clazz == NULL)
        return -3; 
    jmethodID method_id = java_env->GetMethodID(native_activity_clazz, OBFUSCATE("AndroidThunkJava_LaunchURL"), OBFUSCATE("(Ljava/lang/String;)V"));
    if (method_id == NULL)
        return -4;        
    jstring retStr = java_env->NewStringUTF(url);
    java_env->CallVoidMethod(g_App->activity->clazz, method_id, retStr);
    jni_return = java_vm->DetachCurrentThread();
    if (jni_return != JNI_OK)
        return -5;
       return 0;
}
// ======================================================================== //
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))
EGLBoolean (*orig_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface);

    EGLBoolean _eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
        eglQuerySurface(dpy, surface, EGL_WIDTH, &glWidth);
        eglQuerySurface(dpy, surface, EGL_HEIGHT, &glHeight);
        if (glWidth <= 0 || glHeight <= 0)
            return orig_eglSwapBuffers(dpy, surface);

        if (!g_App)
            return orig_eglSwapBuffers(dpy, surface);

        screenWidth = ANativeWindow_getWidth(g_App->window);
        screenHeight = ANativeWindow_getHeight(g_App->window);
        density = AConfiguration_getDensity(g_App->config);

    if (!initImGui) {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplAndroid_Init();
        ImGui_ImplOpenGL3_Init("#version 300 es");
        ImGuiIO & io = ImGui::GetIO();
        io.IniFilename = NULL;
        
        // Use Dear ImGui built-in default font and stock style.
        memset(&Config, 0, sizeof(sConfig));
        
        initImGui = true;
    }

    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(glWidth, glHeight);
    ImGui::NewFrame();
    
    DrawESP(ImGui::GetBackgroundDrawList());

    // The floating logo toggles the native RAMBO menu.
    static bool showModMenu = false;
    if (showModMenu) {
        const float menuWidth = std::min(760.0f, std::max(320.0f, (float)glWidth * 0.62f));
        const float menuHeight = std::min(650.0f, std::max(300.0f, (float)glHeight * 0.82f));
        ImGui::SetNextWindowPos(ImVec2((float)glWidth * 0.5f, (float)glHeight * 0.5f),
                                ImGuiCond_Once, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_Once);

        if (ImGui::Begin("RAMBO Mod Menu", nullptr,
                         ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::TextUnformatted("RAMBO");
            ImGui::SameLine();
            ImGui::TextDisabled("Mod Menu");
            ImGui::Separator();

            if (ImGui::BeginTabBar("##RAMBO_MainTabs", ImGuiTabBarFlags_None)) {
                if (ImGui::BeginTabItem("Dashboard")) {
                    ImGui::Spacing();
                    ImGui::TextUnformatted("Current session");
                    ImGui::Separator();
                    ImGui::Columns(2, "##rambo_status_columns", false);
                    ImGui::TextDisabled("Visual overlay");
                    ImGui::TextUnformatted(Config.ESPMenu.Active ? "Enabled" : "Disabled");
                    ImGui::NextColumn();
                    ImGui::TextDisabled("Aim module");
                    ImGui::TextUnformatted(Config.AimBot.Enable ? "Enabled" : "Disabled");
                    ImGui::Columns(1);
                    ImGui::Spacing();
                    ImGui::Checkbox("Enable visual overlay", &Config.ESPMenu.Active);
                    ImGui::Checkbox("Enable aim module", &Config.AimBot.Enable);
                    ImGui::Checkbox("Automatic fire", &Config.AimMenu.AutoFire);
                    ImGui::Checkbox("Unlimited ammo", &UnlimitedAmmo);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Combat")) {
                    ImGui::Spacing();
                    ImGui::Checkbox("Enable aim module", &Config.AimBot.Enable);
                    ImGui::Checkbox("Automatic fire", &Config.AimMenu.AutoFire);
                    ImGui::Checkbox("Ignore bots", &Config.AimBot.IgnoreBot);
                    ImGui::Checkbox("Ignore knocked players", &Config.AimBot.IgnoreKnocked);
                    ImGui::Checkbox("Visibility check", &Config.AimMenu.VisCheck);
                    ImGui::Separator();
                    static const char* aimTypes[] = { "Distance", "Crosshair" };
                    static const char* aimTargets[] = { "Head", "Chest" };
                    static const char* aimTriggers[] = { "None", "Shooting", "Scoping", "Both", "Any" };
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("Target selection", (int*)&Config.AimMenu.Type, aimTypes, IM_ARRAYSIZE(aimTypes));
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("Target area", (int*)&Config.AimMenu.Target, aimTargets, IM_ARRAYSIZE(aimTargets));
                    ImGui::SetNextItemWidth(-1.0f);
                    ImGui::Combo("Activation", (int*)&Config.AimMenu.Trigger, aimTriggers, IM_ARRAYSIZE(aimTriggers));
                    if (Config.AimMenu.Type == EAimType::Croshhair) {
                        ImGui::SetNextItemWidth(-1.0f);
                        ImGui::SliderFloat("Field of view", &Config.AimMenu.FOVSize, 0.0f, 500.0f, "%.0f");
                    }
                    ImGui::Checkbox("Fast fire control", &FASTSHOOT);
                    if (FASTSHOOT) {
                        ImGui::SetNextItemWidth(-1.0f);
                        ImGui::SliderFloat("Fire control", &ControlSpeed, 0.0f, 0.70f, "%.2f");
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Visuals")) {
                    ImGui::Spacing();
                    ImGui::Checkbox("Enable visual overlay", &Config.ESPMenu.Active);
                    ImGui::Separator();
                    ImGui::Columns(2, "##rambo_visual_columns", false);
                    ImGui::Checkbox("Lines", &Config.ESPMenu.Line);
                    ImGui::Checkbox("Boxes", &Config.ESPMenu.Box);
                    ImGui::Checkbox("Skeleton", &Config.ESPMenu.Skeleton);
                    ImGui::Checkbox("Names", &Config.ESPMenu.Name);
                    ImGui::Checkbox("Health", &Config.ESPMenu.Health);
                    ImGui::Checkbox("Distance", &Config.ESPMenu.Distance);
                    ImGui::NextColumn();
                    ImGui::Checkbox("Enemy weapon", &Config.ESPMenu.EnemyWeapon);
                    ImGui::Checkbox("Vehicles", &Config.ESPMenu.Vehicle);
                    ImGui::Checkbox("Loot boxes", &Config.ESPMenu.LootBox);
                    ImGui::Checkbox("Alerts", &Config.ESPMenu.Alert);
                    ImGui::Checkbox("Game information", &Config.ESPMenu.GameInfo);
                    ImGui::Columns(1);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Misc")) {
                    ImGui::Spacing();
                    ImGui::Columns(2, "##rambo_misc_columns", false);
                    ImGui::Checkbox("Flash setting", &Config.HighRisk.Flash);
                    ImGui::Checkbox("Parachute", &Config.HighRisk.Parachute);
                    ImGui::Checkbox("Unlimited ammo logic", &Config.HighRisk.UnlimitedAmmo1);
                    ImGui::Checkbox("Sky movement", &Config.HighRisk.SkyFly);
                    ImGui::Checkbox("Wide view", &Config.HighRisk.WideView);
                    ImGui::NextColumn();
                    ImGui::Checkbox("No fog", &Config.HighRisk.NoFog);
                    ImGui::Checkbox("Recoil setting", &Config.HighRisk.Recoil);
                    ImGui::Checkbox("Camera shake", &Config.HighRisk.Shake);
                    ImGui::Checkbox("Hit effect", &Config.HighRisk.HitEffect);
                    ImGui::Checkbox("Kill message", &Config.HighRisk.KillMsg);
                    ImGui::Columns(1);
                    ImGui::Separator();
                    ImGui::Checkbox("Unlimited ammo", &UnlimitedAmmo);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Settings")) {
                    ImGui::Spacing();
                    ImGui::TextUnformatted("Interface information");
                    ImGui::Separator();
                    ImGui::TextDisabled("Theme: Dear ImGui default dark");
                    ImGui::TextDisabled("Renderer: OpenGL ES 3");
                    ImGui::TextDisabled("Architecture: arm64-v8a");
                    ImGui::TextDisabled("Menu: native RAMBO.cpp");
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("About")) {
                    ImGui::Spacing();
                    ImGui::TextUnformatted("RAMBO Mod Menu");
                    ImGui::Separator();
                    ImGui::TextWrapped("Native ImGui interface. The floating logo opens and closes this menu.");
                    ImGui::TextWrapped("The interface uses the built-in Dear ImGui dark preset without custom palette overrides.");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    // Keep the launcher above the menu window so it remains tappable while open.
    const float logoSize = std::max(48.0f, (float)density / 8.0f);
    const float logoMargin = std::max(8.0f, (float)density / 28.0f);
    const float logoX = std::max(4.0f, (float)glWidth - logoSize - logoMargin);
    const float logoY = std::max(4.0f, (float)glHeight * 0.40f);
    ImGui::SetNextWindowPos(ImVec2(logoX, logoY), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    if (ImGui::Begin("##RAMBO_FloatingLogo", nullptr,
                     ImGuiWindowFlags_NoDecoration |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoFocusOnAppearing |
                     ImGuiWindowFlags_NoNav |
                     ImGuiWindowFlags_AlwaysAutoResize)) {
        if (RamboFloatingLogo::EnsureLoaded()) {
            if (ImGui::ImageButton((ImTextureID)(intptr_t)RamboFloatingLogo::gTexture,
                                   ImVec2(logoSize, logoSize),
                                   ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f),
                                   0, ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
                                   ImVec4(1.0f, 1.0f, 1.0f, 1.0f))) {
                showModMenu = !showModMenu;
            }
        } else {
            // Icon-only fallback using the stock ImGui palette; never show text.
            ImGui::InvisibleButton("##RAMBO_FloatingLogoFallback", ImVec2(logoSize, logoSize));
            if (ImGui::IsItemClicked()) {
                showModMenu = !showModMenu;
            }
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            const ImVec2 itemMin = ImGui::GetItemRectMin();
            const ImVec2 itemMax = ImGui::GetItemRectMax();
            const ImVec2 center((itemMin.x + itemMax.x) * 0.5f, (itemMin.y + itemMax.y) * 0.5f);
            const float radius = logoSize * 0.42f;
            drawList->AddCircleFilled(center, radius, ImGui::GetColorU32(ImGuiCol_Button), 32);
            drawList->AddCircle(center, radius, ImGui::GetColorU32(ImGuiCol_Border), 32, 1.5f);
            drawList->AddCircle(center, radius * 0.40f, ImGui::GetColorU32(ImGuiCol_Text), 32, 1.5f);
            drawList->AddLine(ImVec2(center.x - radius * 0.72f, center.y),
                              ImVec2(center.x + radius * 0.72f, center.y),
                              ImGui::GetColorU32(ImGuiCol_Text), 1.5f);
            drawList->AddLine(ImVec2(center.x, center.y - radius * 0.72f),
                              ImVec2(center.x, center.y + radius * 0.72f),
                              ImGui::GetColorU32(ImGuiCol_Text), 1.5f);
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return orig_eglSwapBuffers(dpy, surface);
}

int32_t (*orig_onInputEvent)(struct android_app *app, AInputEvent *inputEvent);
int32_t onInputEvent(struct android_app *app, AInputEvent *inputEvent) {
    if (initImGui) {
        ImGui_ImplAndroid_HandleInputEvent(inputEvent, {(float) screenWidth / (float) glWidth, (float) screenHeight / (float) glHeight});
    }
    return orig_onInputEvent(app, inputEvent);
}


void *(*oProcessEvent)(UObject *pObj, UFunction *pFunc, void *pArgs);
void *hkProcessEvent(UObject *pObj, UFunction *pFunc, void *pArgs) {
    const char *EngineHUD = ("Function Engine.HUD.ReceiveDrawHUD");
    if (pFunc) {
        if (pFunc->GetFullName() == EngineHUD) {
            AHUD *pHUD = (AHUD *) pObj;
            if (pHUD) {

                auto Params = (AHUD_ReceiveDrawHUD_Params *) pArgs;
                if (Params) {
                    RenderESPPRIVATE(pHUD, Params->SizeX, Params->SizeY);
             }
            }
        }
    }
    return oProcessEvent(pObj, pFunc, pArgs);
}




//=====================================================================================//
 

#define SLEEP_TIME 1000LL / 60LL
[[noreturn]] void *maps_thread(void *) {
    while (true) {
    auto t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::vector<sRegion> tmp;
    char line[512];
        FILE *f = fopen("/proc/self/maps", "r");
        if (f)
        {
            while (fgets(line, sizeof line, f))
            {
                uintptr_t start, end;
                char tmpProt[16];
                if (sscanf(line, "%" PRIXPTR "-%" PRIXPTR " %16s %*s %*s %*s %*s", &start, &end, tmpProt) > 0)
                {
                    if (tmpProt[0] != 'r')
                    {
                        tmp.push_back({start, end});
                    }
                }
            }
            fclose(f);
        }
    trapRegions = tmp;
    auto localPlayer = g_LocalPlayer;
        if (localPlayer) {
            auto WeaponManagerComponent = localPlayer->WeaponManagerComponent;
            if (WeaponManagerComponent) {
                auto CurrentWeaponReplicated = (ASTExtraShootWeapon *)WeaponManagerComponent->CurrentWeaponReplicated;
                if (CurrentWeaponReplicated) {
                    auto ShootWeaponEntityComp = CurrentWeaponReplicated->ShootWeaponEntityComp;
                    auto ShootWeaponEffectComp = CurrentWeaponReplicated->ShootWeaponEffectComp;
                    if (ShootWeaponEntityComp && ShootWeaponEffectComp) {
                       if (Config.HighRisk.Flash) {
                            g_LocalPlayer->StandHalfHeight = 77.0f;
                            g_LocalPlayer->CrouchHalfHeight = 30.0f;
                                          } else {
                     		g_LocalPlayer->StandHalfHeight = 88.0f;
                            g_LocalPlayer->CrouchHalfHeight = 60.0f;
                        }  
                    }
                }
            }
        }


    auto object = UObject::GetGlobalObjects();
    for (int i = 0; i < object.Num(); i++){
    auto Object = object.GetByIndex(i);
    if (isObjectInvalid(Object))
        continue;
		
		if (Config.HighRisk.Flash) {
    if (Object->IsA(AWorldSettings::StaticClass())) {
        auto playerChar = (AWorldSettings *) Object;      
        playerChar->MinUndilatedFrameTime = 0.100f;
		HOOK((UE4 + 0x3503040), hsub_3503040, &sub_3503040);
    }} else if (Object->IsA(AWorldSettings::StaticClass())) {
        auto playerChar = (AWorldSettings *) Object;
        playerChar->MinUndilatedFrameTime = 0;
		HOOK((UE4 + 0x3503040), YT_3503040, &BAM_3503040);
    }
		
    
    // --- Apply WideView Logic ---
if (Config.HighRisk.WideView) {
    if (Object->IsA(ULocalPlayer::StaticClass())) {
    auto playerChar = (ULocalPlayer *) Object;      
    playerChar->AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MaintainYFOV;
    }} else if (Object->IsA(ULocalPlayer::StaticClass())) {
    auto playerChar = (ULocalPlayer *) Object;
    playerChar->AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MaintainXFOV;
    }
	
	
    if (Config.HighRisk.NoFog) {
    if (Object->IsA(UExponentialHeightFogComponent::StaticClass())) {            
    auto playerChar = (UExponentialHeightFogComponent *) Object;
    playerChar->SetStartDistance(0);
    playerChar->SetFogMaxOpacity(0);
    playerChar->SetFogHeightFalloff(0);
    playerChar->SetFogDensity(0);
    playerChar->SetFogCutoffDistance(0); 
    }}}
    
    
	
if (Config.HighRisk.STUCK) {
g_LocalPlayer->StandHalfHeight = 77.0f;
g_LocalPlayer->CrouchHalfHeight = 50.0f;
}


if (Config.HighRisk.UnlimitedAmmo1) {
        auto objs = UObject::GetGlobalObjects();
        for (int i = 0; i < objs.Num(); i++) {
            auto Object = objs.GetByIndex(i);
   if (isObjectInvalid(Object))
                continue;

if (Object->IsA(UShootWeaponEntity::StaticClass())) {
               auto SDK = (UShootWeaponEntity *) Object;
               SDK->bHasInfiniteClips = true;
               SDK->bClipHasInfiniteBullets = true;
            }     
            if (Object->IsA(ASTExtraShootWeapon::StaticClass())) {
               auto SDK = (ASTExtraShootWeapon *) Object;
               SDK->CurMaxBulletNumInOneClip = 6969;
            }
     }
}
                                           
        auto td = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t1;
        std::this_thread::sleep_for(std::chrono::milliseconds(std::max(std::min(0LL, SLEEP_TIME - td), SLEEP_TIME)));
    }
}

void * main_thread(void *) {

    while (!UE4) {
        UE4 = Tools::GetBaseAddress("libUE4.so");
        sleep(1);
    }
    

    while (!g_App) {
        g_App = *(android_app **) (UE4 + GNativeApp_Offset);
        sleep(1);
    }
    
    FName::GNames = GetGNames();
    while (!FName::GNames) {
        FName::GNames = GetGNames();
        sleep(1);
    }
    UObject::GUObjectArray = (FUObjectArray *) (UE4 + GUObject_Offset);
    
    orig_onInputEvent = decltype(orig_onInputEvent)(g_App->onInputEvent);
    g_App->onInputEvent = onInputEvent;
    
    void *egl = dlopen_ex("libEGL.so", 4);
    if (egl != nullptr) {
        void *swapBuffers = dlsym_ex(egl, "eglSwapBuffers");
        if (swapBuffers != nullptr) {
            HOOK(swapBuffers, _eglSwapBuffers, &orig_eglSwapBuffers);
        }
        dlclose_ex(egl);
    }
	
Tools::Hook((void *)(UE4 + 0x1F5E2E8), (void *) CalcShootRot, (void **) &oCalcShootRot);
Tools::Hook((void *) (UE4 + 0x294173C), (void *) hkProcessEvent,(void **) &oProcessEvent);
MemoryPatch::createWithHex("libCrashSight.so", 0x1DA8, "00 00 00 00",4).Modify();
MemoryPatch::createWithHex("libCrashSight.so", 0x1DC0, "00 00 00 00",4).Modify();
MemoryPatch::createWithHex("libCrashSight.so", 0x1E08, "00 00 00 00",4).Modify();
MemoryPatch::createWithHex("libCrashSight.so", 0x1EC8, "00 00 00 00",4).Modify();
Patches.Flash01 = MemoryPatch::createWithHex("libUE4.so", 0x3A662A0, "00 00 00 00 00 00 10 41"); // desert map
Patches.Flash02 = MemoryPatch::createWithHex("libUE4.so", 0x3A66394, "1f 20 03 d5 4a f8 ff 5c"); // color x hit
Patches.Flash03 = MemoryPatch::createWithHex("libUE4.so", 0x3B56C30, "e3 03 27 1e");
Patches.Flash04 = MemoryPatch::createWithHex("libUE4.so", 0x3B56C2C, "17 00 80 52");
Patches.Flash05 = MemoryPatch::createWithHex("libUE4.so", 0x34E85F0, "17 00 80 52");
    items_data = json::parse(JSON_ITEMS);
    
    
    pthread_t t;
    pthread_create(&t, 0, maps_thread, 0);
    items_data = json::parse(JSON_ITEMS);
     
    return 0;
}

__attribute__((constructor)) void _init(){
    pthread_t t;
    pthread_create(&t, 0, main_thread, 0);
}

