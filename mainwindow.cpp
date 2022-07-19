#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <iostream>
#include <string>
#include <random>
#include <ctime>
struct hero_status
{
    float hp = 0;
    int atk = 0;
    int def = 0;
    int spd = 0;
    float shield = 0;
    bool shield_on = false;
    float prev = 0;
    bool prevent = false;
    float reduce = 0;
};
struct hero_debuff
{
    int atkdown = 0;
    bool silence = false;
    bool sleep = false;
    bool confusion = false;
    int tear = 0;
    bool coma = false;
    bool coma_round = false;
};
//随机工具
std::default_random_engine e;
std::uniform_real_distribution<float> u;

//不知所云的父类
class hero
{
public:
    hero_status status;
    hero_debuff debuff;
    bool live = true;
    bool silent;
    std::string name;
    virtual bool attack(hero *enemy, int round) { return false; }
    virtual void init(bool ifsilent){};
    virtual void hero_on_round(int round){};
    bool on_round(int round) //有人死亡返回true 回合开始结算
    {
        hero_on_round(round);
        if (status.prevent)
        {
            status.prevent = false;
        }
        if (debuff.silence)
        {
            debuff.silence = false;
        }
        if (debuff.sleep)
        {
            debuff.sleep = false;
        }
        if (debuff.tear > 0)
        {
            debuff.tear--;
            this->status.hp -= 4;
            if (!silent)
            {
                std::cout << std::endl
                          << " " << name << "被撕裂，流失4点生命";
            }
            if ((int)status.hp < 1)
            {
                this->live = false;
                return true;
            }
        }
        if (debuff.coma_round)
        {
            debuff.coma = false;
            debuff.coma_round = false;
        }
        return false;
    }
    bool before_move(char move)
    { //结算debuff 跳过则false 1普攻 2主动 3被动
        if (move == 1)
        {
            if (debuff.atkdown == 1)
            {
                status.atk = status.atk - 6;
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "攻击力-6";
                }
                debuff.atkdown = 2;
            }
            if (debuff.atkdown == 2)
            {
                status.atk = status.atk + 6;
                debuff.atkdown = 0;
            }
            if (debuff.sleep)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "行动被封印";
                }
                return false;
            }
            if (debuff.coma)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "昏迷无法行动";
                }
                debuff.coma_round = true;
                return false;
            }
        }
        else if (move == 2)
        {
            if (debuff.atkdown == 1)
            {
                status.atk = status.atk - 6;
                if (!silent)
                {
                    std::cout << " " << name << "攻击力-6";
                }
                debuff.atkdown = 2;
            }
            if (debuff.atkdown == 2)
            {
                status.atk = status.atk + 6;
                debuff.atkdown = 0;
            }
            if (debuff.sleep)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "行动被封印";
                }
                return false;
            }
            if (debuff.silence)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "沉默无法施放技能";
                }
                return false;
            }
            if (debuff.coma)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "昏迷无法行动";
                }
                debuff.coma_round = true;
                return false;
            }
        }
        else if (move == 3)
        {
            if (debuff.sleep)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "行动被封印";
                }
                return false;
            }
            if (debuff.silence)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << " " << name << "沉默无法施放技能";
                }
                return false;
            }
        }
        return true;
    };
    void hp_add(hero *enemy, float num)
    {
        int before = enemy->status.hp;
        enemy->status.hp = enemy->status.hp + num;
        if (enemy->status.hp > 100)
        {
            enemy->status.hp = 100;
        }
        if (!silent)
        {
            std::cout << " " << enemy->name << "回复" << (int)enemy->status.hp - before << "点生命";
        }
    }
    virtual bool cause_damage(hero *enemy, bool elemental, float num, bool defentable) //有人死亡返回true
    {
        if (enemy->status.prevent && defentable) //免疫伤害
        {
            if (!silent)
            {
                std::cout << " 伤害被免疫";
            }
            return false;
        }
        if (enemy->status.prev > 0.01 && defentable)
        {
            if (u(e) < enemy->status.prev)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << "[樱：夏之型-樱流] 闪避本回合攻击";
                }
                enemy->status.prevent = true;
                return false;
            }
        }
        float damage; //计算伤害
        if (elemental)
        {
            damage = num * (1 - enemy->status.reduce);
        }
        else
        {
            damage = (num - enemy->status.def) * (1 - enemy->status.reduce);
        }
        if (damage < 0)
        {
            damage = 0;
        }
        //结算伤害
        if (enemy->status.shield_on) //格蕾修有盾
        {
            if (damage <= enemy->status.shield)
            {
                int before = enemy->status.shield;
                enemy->status.shield = enemy->status.shield - damage;
                if (!silent)
                {
                    std::cout << " 格蕾修护盾抵挡" << before - (int)enemy->status.shield << "点伤害";
                }
                return false;
            }
            enemy->status.shield_on = false;
            damage = damage - enemy->status.shield;
            int before = enemy->status.hp;
            enemy->status.hp = enemy->status.hp - damage;
            if (!silent)
            {
                std::cout << " 格蕾修受到" << before - (int)enemy->status.hp << "点伤害";
            }
            if ((int)enemy->status.hp < 1)
            {
                enemy->live = false;
                return true;
            }
            float reflexdam = (2 + 2 * u(e)) * enemy->status.def;
            if (reflexdam < 0)
            {
                reflexdam = 0;
            }
            before = status.hp;
            status.hp = status.hp - reflexdam;
            if (!silent)
            {
                std::cout << std::endl
                          << "[格蕾修：水彩泡影] " << name << "受到" << before - (int)status.hp << "点反弹伤害";
            }
            if ((int)status.hp < 1)
            {
                live = false;
                return true;
            }
            return false;
        }
        //正常结算 md上面好恶心
        int before = enemy->status.hp;
        enemy->status.hp = enemy->status.hp - damage;
        if (!silent)
        {
            std::cout << " " << enemy->name << "受到" << before - (int)enemy->status.hp << "点伤害";
        }
        if ((int)enemy->status.hp < 1)
        {
            enemy->live = false;
            return true;
        }
        return false;
    }
};
//挂比凯文
class kw : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 20;
        status.def = 11;
        status.spd = 21;
        name = "凯文";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 3 == 0)
        {
            if (before_move(2))
            {
                damage = 25;
                status.atk += 5;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[凯文：清凉一剑] 攻击力永久+5";
                }
                if (cause_damage(enemy, true, damage, true))
                {
                    return true;
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[凯文：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        if (before_move(3))
        {
            if (debuff.coma)
            {
                return false;
            }
            if (enemy->status.hp < 30 && u(e) < 0.3)
            {
                enemy->live = false;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[凯文：炎热归零] 凯文将" << enemy->name << "秒杀";
                }
                return true;
            }
        }
        return false;
    };
};
//爱莉希雅
class alxy : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 21;
        status.def = 8;
        status.spd = 20;
        name = "爱莉希雅";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 2 == 0)
        {
            if (before_move(2))
            {
                damage = 25 + 25 * u(e);
                enemy->debuff.atkdown = 1;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[爱莉希雅：夏梦之花] 对手下次行动攻击力-6";
                }
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[爱莉希雅：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        if (before_move(3))
        {
            if (debuff.coma)
            {
                return false;
            }
            if (u(e) < 0.35)
            {
                damage = 11;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[爱莉希雅：水花溅射]";
                }
                if (cause_damage(enemy, true, damage, true))
                {
                    return true;
                }
            }
        }
        return false;
    };
};
//阿波尼亚
class abny : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 21;
        status.def = 10;
        status.spd = 30;
        name = "阿波尼亚";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 4 == 0)
        {
            if (before_move(2))
            {
                damage = floor(status.atk * 1.7);
                if (!silent)
                {
                    std::cout << std::endl
                              << "[阿波尼亚：深蓝之槛] 对手本回合封印";
                }
                enemy->debuff.sleep = true;
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[阿波尼亚：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        if (before_move(3))
        {
            if (debuff.coma)
            {
                return false;
            }
            if (u(e) < 0.3)
            {
                if (debuff.confusion)
                {
                    this->debuff.silence = true;
                    if (!silent)
                    {
                        std::cout << std::endl
                                  << "[阿波尼亚：该休息了] 阿波尼亚将" << this->name << "沉默";
                    }
                    debuff.confusion = false;
                }
                else
                {
                    enemy->debuff.silence = true;
                    if (!silent)
                    {
                        std::cout << std::endl
                                  << "[阿波尼亚：该休息了] 阿波尼亚将" << enemy->name << "沉默";
                    }
                }
            }
        }
        return false;
    };
};
//维尔薇
class wlv : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 20;
        status.def = 12;
        status.spd = 25;
        name = "维尔薇";
    };
    virtual void hero_on_round(int round)
    {
        if (status.hp < 31 && vlv)
        {
            vlv = false;
            if (!silent)
            {
                std::cout << std::endl
                          << "[维尔薇：大变活人] ";
            }
            hp_add(this, 10 + 10 * u(e));
            hp_add(_enemy, 10 + 10 * u(e));
            int atkp = 2 + 13 * u(e);
            if (!silent)
            {
                std::cout << "维尔薇永久增加" << atkp << "攻击力";
            }
            this->status.atk += atkp;
        }
    }
    virtual bool attack(hero *enemy, int round)
    {
        _enemy = enemy;
        float damage;
        if (round % 3 == 0)
        {
            if (before_move(2))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[维尔薇：创（造）力] 使对手混乱";
                }
                enemy->debuff.confusion = true;
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[维尔薇：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        //////////////
        return false;
    };

private:
    bool vlv = true;
    hero *_enemy;
};
//千劫
class qj : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 23;
        status.def = 9;
        status.spd = 26;
        name = "千劫";
    };
    virtual bool attack(hero *enemy, int round)
    {
        qianjie();
        float damage;
        if (round % 3 == 0 && (int)status.hp > 10)
        {
            if (before_move(2))
            {
                float damage = 45;
                float damageE = 1 + 19 * u(e);
                status.hp -= 10;
                status.atk += 2;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[千劫：盛夏燔祭] 对自己造成10点伤害";
                }
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
                if (cause_damage(enemy, true, damageE, true))
                {
                    return true;
                }
            }
        }
        else if (round % 3 == 1 && round > 3)
        {
            if (!silent)
            {
                std::cout << std::endl
                          << "[千劫：盛夏燔祭] 千劫休息";
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[千劫：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        if (before_move(3))
        {
            if (u(e) < 0.35)
            {
                damage = 11;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[千劫：普通攻击]";
                }
                if (cause_damage(enemy, true, damage, true))
                {
                    return true;
                }
            }
        }
        return false;
    };

private:
    void qianjie()
    {
        this->status.atk = 23 + (100 - (int)status.hp) / 5;
    }
};
//樱
class skr : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 24;
        status.def = 10;
        status.spd = 27;
        status.prev = 0.15;
        name = "樱";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 2 == 0)
        {
            if (before_move(2))
            {
                damage = (int)(1.3 * status.atk);
                if (!silent)
                {
                    std::cout << std::endl
                              << "[樱：夏之型：瓜切]";
                    hp_add(this, 1 + 4 * u(e));
                }
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[樱：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        return false;
    };
};
//科斯魔
class ksm : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 19;
        status.def = 11;
        status.spd = 19;
        name = "科斯魔";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 2 == 0)
        {
            if (before_move(2))
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << "[科斯魔：邪渊之钩]";
                }
                for (int t = 0; t < 4; t++)
                {
                    damage = 11 + 11 * u(e);
                    if (cause_damage(enemy, false, damage, true))
                    {
                        return true;
                    }
                    if (enemy->debuff.tear > 0)
                    {
                        if (cause_damage(enemy, true, 3, true))
                        {
                            return true;
                        }
                    }
                    if (u(e) < 0.15)
                    {
                        if (!silent)
                        {
                            std::cout << std::endl
                                      << "[科斯魔：不归之爪] " << enemy->name << "被撕裂";
                        }
                        enemy->debuff.tear = 3;
                    }
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[科斯魔：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    if (u(e) < 0.15)
                    {
                        if (!silent)
                        {
                            std::cout << std::endl
                                      << "[科斯魔：不归之爪] " << this->name << "被撕裂";
                        }
                        enemy->debuff.tear = 3;
                    }
                    debuff.confusion = false;
                }
                else
                {
                    if (cause_damage(enemy, false, damage, true))
                    {
                        return true;
                    }
                    if (u(e) < 0.15)
                    {
                        if (!silent)
                        {
                            std::cout << std::endl
                                      << "[科斯魔：不归之爪] " << enemy->name << "被撕裂";
                        }
                        enemy->debuff.tear = 3;
                    }
                }
            }
        }
        return false;
    };
};
//梅比乌斯
class mbws : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 21;
        status.def = 11;
        status.spd = 23;
        name = "梅比乌斯";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 3 == 0)
        {
            if (before_move(2))
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << "[梅比乌斯：栖影水枪]";
                }
                if (cause_damage(enemy, false, 33, true))
                {
                    return true;
                }
                if (u(e) < 0.33)
                {
                    enemy->debuff.coma = true;
                    if (!silent)
                    {
                        std::cout << std::endl
                                  << "[梅比乌斯：栖影水枪] 使" << enemy->name << "昏迷";
                    }
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[梅比乌斯：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    if (u(e) < 0.33)
                    {
                        if (!silent)
                        {
                            std::cout << std::endl
                                      << "[梅比乌斯：不稳定物质] 使梅比乌斯防御力永久下降3点";
                        }
                        status.def -= 3;
                        if (status.def < 0)
                        {
                            status.def = 0;
                        }
                    }
                    debuff.confusion = false;
                }
                else
                {
                    if (cause_damage(enemy, false, damage, true))
                    {

                        return true;
                    }
                    if (u(e) < 0.33)
                    {
                        if (!silent)
                        {
                            std::cout << std::endl
                                      << "[梅比乌斯：不稳定物质] 使" << enemy->name << "防御力永久下降3点";
                        }
                        enemy->status.def -= 3;
                        if (enemy->status.def < 0)
                        {
                            enemy->status.def = 0;
                        }
                    }
                }
            }
        }
        return false;
    };
};
//格蕾修
class glx : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 16;
        status.def = 11;
        status.spd = 18;
        name = "格蕾修";
    };
    virtual bool attack(hero *enemy, int round)
    {
        float damage;
        if (round % 3 == 0)
        {
            if (before_move(2))
            {
                damage = status.def;
                if (status.shield_on)
                {
                    if (!silent)
                    {
                        std::cout << std::endl
                                  << "[格蕾修：水彩泡影] ";
                    }
                    if (cause_damage(enemy, false, damage, true))
                    {
                        return true;
                    }
                }
                else
                {
                    status.shield_on = true;
                    int num = 15;
                    status.shield = num;
                    if (!silent)
                    {
                        std::cout << std::endl
                                  << "[格蕾修：水彩泡影] 格蕾修获得15点护盾";
                    }
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[格蕾修：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        if (before_move(3))
        {
            if (debuff.coma)
            {
                return false;
            }
            if (u(e) < 0.4)
            {
                if (glxnum < 10)
                {
                    status.def += 2;
                    glxnum += 2;
                    if (!silent)
                    {
                        std::cout << std::endl
                                  << "[格蕾修：沙滩监护人] 格蕾修防御+2";
                    }
                }
            }
        }
        return false;
    };

private:
    int glxnum = 0;
};
//华
class hua : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 21;
        status.def = 12;
        status.spd = 15;
        status.reduce = 0.2;
        name = "华";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 2 == 0)
        {
            if (before_move(2))
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << "[华：上伞若水] 华开始蓄力";
                }
                if (!hua)
                {
                    hua = true;
                    status.def += 3;
                }
            }
        }
        else
        {
            if (before_move(1))
            {
                if (hua)
                {
                    hua = false;
                    status.def -= 3;
                }
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[华：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
                if(round>1){
                if (!silent)
                {
                    std::cout << std::endl
                              << "[华：上伞若水] ";
                }
                if (cause_damage(enemy, true, 10 + 23 * u(e), true))
                {
                    return true;
                }
                }
            }
        }
        return false;
    };

private:
    bool hua = false;
};
//猫猫
class mm : public hero
{
public:
    virtual bool cause_damage(hero *enemy, bool elemental, float num, bool defentable) //有人死亡返回true
    {
        if (enemy->status.prevent && defentable) //免疫伤害
        {
            if (!silent)
            {
                std::cout << " 伤害被免疫";
            }
            mmnum = 0;
            return false;
        }
        if (enemy->status.prev > 0.01 && defentable)
        {
            if (u(e) < enemy->status.prev)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << "[樱：夏之型-樱流] 闪避本回合攻击";
                }
                enemy->status.prevent = true;
                mmnum = 0;
                return false;
            }
        }
        float damage; //计算伤害
        if (elemental)
        {
            damage = num * (1 - enemy->status.reduce);
        }
        else
        {
            damage = (num - enemy->status.def) * (1 - enemy->status.reduce);
        }
        if (damage < 0)
        {
            damage = 0;
        }
        //结算伤害
        if (enemy->status.shield_on) //格蕾修有盾
        {
            if (damage <= enemy->status.shield)
            {
                int before = enemy->status.shield;
                enemy->status.shield = enemy->status.shield - damage;
                if (!silent)
                {
                    std::cout << " 格蕾修护盾抵挡" << before - (int)enemy->status.shield << "点伤害";
                }
                mmnum = before - (int)enemy->status.shield;
                return false;
            }
            damage = damage - enemy->status.shield;
            int before = enemy->status.hp;
            enemy->status.hp = enemy->status.hp - damage;
            if (!silent)
            {
                std::cout << " 格蕾修受到" << before - (int)enemy->status.hp << "点伤害";
                mmnum = before - (int)enemy->status.hp;
            }
            if ((int)enemy->status.hp < 1)
            {
                enemy->live = false;
                return true;
            }
            float reflexdam = (2 + 2 * u(e)) * enemy->status.def;
            if (reflexdam < 0)
            {
                reflexdam = 0;
            }
            before = status.hp;
            status.hp = status.hp - reflexdam;
            if (!silent)
            {
                std::cout << std::endl
                          << "[格蕾修：水彩泡影] " << name << "受到" << before - (int)status.hp << "点反弹伤害";
            }
            if ((int)status.hp < 1)
            {
                live = false;
                return true;
            }
            return false;
        }
        //正常结算 md上面好恶心
        int before = enemy->status.hp;
        enemy->status.hp = enemy->status.hp - damage;
        if (!silent)
        {
            std::cout << " " << enemy->name << "受到" << before - (int)enemy->status.hp << "点伤害";
        }
        if ((int)enemy->status.hp < 1)
        {
            enemy->live = false;
            return true;
        }
        mmnum = before - (int)enemy->status.hp;
        return false;
    }
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 17;
        status.def = 10;
        status.spd = 24;
        name = "帕朵菲莉丝";
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 3 == 0)
        {
            if (before_move(2))
            {
                damage = 30;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[帕朵菲莉丝:沙滩寻宝]";
                }
                mmnum = 0;
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
                hp_add(this, mmnum);
            }
        }
        else
        {
            if (before_move(1))
            {
                damage = status.atk;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[帕朵菲莉丝：普通攻击]";
                }
                if (debuff.confusion)
                {
                    if (cause_damage(this, false, damage, true))
                    {
                        return true;
                    }
                    debuff.confusion = false;
                }
                else if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        if (before_move(3))
        {
            if (debuff.coma)
            {
                return false;
            }
            if (u(e) < 0.3)
            {
                damage = 30;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[帕朵菲莉丝：最佳拍档]";
                }
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        return false;
    };

private:
    int mmnum;
};
//伊甸
class yd : public hero
{
public:
    virtual void init(bool ifsilent)
    {
        silent = ifsilent;
        status.hp = 100;
        status.atk = 16;
        status.def = 12;
        status.spd = 16;
        name = "伊甸";
    };
    virtual void hero_on_round(int round)
    {
        if (status.spd == 50)
        {
            status.spd = 16;
        }
        if (status.spd == 51)
        {
            status.spd = 50;
        }
    };
    virtual bool attack(hero *enemy, int round)
    {

        float damage;
        if (round % 2 == 0)
        {
            if (before_move(2))
            {
                status.atk += 4;
                status.spd = 51;
                if (!silent)
                {
                    std::cout << std::endl
                              << "[伊甸：闪亮登场] 攻击力永久+4 下回合获得先攻";
                }
            }
        }
        if (before_move(1))
        {
            damage = status.atk;
            if (!silent)
            {
                std::cout << std::endl
                          << "[伊甸：普通攻击]";
            }
            if (debuff.confusion)
            {
                if (cause_damage(this, false, damage, true))
                {
                    return true;
                }
                debuff.confusion = false;
            }
            else if (cause_damage(enemy, false, damage, true))
            {
                return true;
            }
            if (u(e) < 0.5)
            {
                if (!silent)
                {
                    std::cout << std::endl
                              << "[伊甸：海边协奏]";
                }
                if (cause_damage(enemy, false, damage, true))
                {
                    return true;
                }
            }
        }
        return false;
    }
};
void helper()
{
    std::cout << "崩坏3 2022 夏活 海滨乱斗 SIMULITOR" << std::endl;
    std::cout << "                       --sdfs     " << std::endl;
    std::cout << "usage:                            " << std::endl;
    std::cout << "main num1 num2                    " << std::endl;
    std::cout << "    指定num1和num2编号英桀模拟乱斗" << std::endl;
    std::cout << "main num                          " << std::endl;
    std::cout << " 所有英桀两两作战各num次并统计结果" << std::endl;
    std::cout << "英桀编号:                         " << std::endl;
    std::cout << " 1 凯文                           " << std::endl;
    std::cout << " 2 爱莉希雅                       " << std::endl;
    std::cout << " 3 阿波尼亚                       " << std::endl;
    std::cout << " 4 伊甸                           " << std::endl;
    std::cout << " 5 维尔薇                         " << std::endl;
    std::cout << " 6 千劫                           " << std::endl;
    std::cout << " 7 樱                             " << std::endl;
    std::cout << " 8 科斯魔                         " << std::endl;
    std::cout << " 9 梅比乌斯                       " << std::endl;
    std::cout << " 10 格蕾修                        " << std::endl;
    std::cout << " 11 华                            " << std::endl;
    std::cout << " 12 帕朵菲莉丝                    " << std::endl;
}
hero *herofac(int num)
{
    switch (num)
    {
    case 1:
        return new kw;
    case 2:
        return new alxy;
    case 3:
        return new abny;
    case 4:
        return new yd;
    case 5:
        return new wlv;
    case 6:
        return new qj;
    case 7:
        return new skr;
    case 8:
        return new ksm;
    case 9:
        return new mbws;
    case 10:
        return new glx;
    case 11:
        return new hua;
    case 12:
        return new mm;
    default:
        return nullptr;
        break;
    }
}


//输出重定向
class Mybuf:public std::streambuf{
public:
    Mybuf(QPlainTextEdit* text){
        buffer.reserve(8);
        textout = text;
    }
protected:
    int overflow(int c = EOF){
       if(c!=EOF){
           buffer.push_back((char)c);

       }
       return c;
    }
    int sync(){
        if(!buffer.empty()){
            textout->insertPlainText(QString::fromStdString(buffer));
            buffer.clear();
        }
        return 0;
    }
private:
    QPlainTextEdit *textout;
    std::string buffer;
};


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    u = std::uniform_real_distribution<float>(0, 1);
    e.seed(time(0));
    ui->setupUi(this);
    Mybuf *buf = new Mybuf(ui->plainTextEdit);
    ui->plainTextEdit->setReadOnly(true);
    std::cout.rdbuf(buf);
    std::cout<<"输出已经重定向到UI"<<std::endl;
    std::cout<<"欢迎使用千界一乘模拟器"<<std::endl;
    std::cout<<"    developer:sdfs"<<std::endl;
    std::cout<<"                  "<<std::endl;
    std::cout<<"左侧选择英桀和模式"<<std::endl;
    std::cout<<"注：若超过100回合未分胜负视为双方胜利"<<std::endl;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_checkBox_stateChanged(int arg1)
{
    qjyc = arg1;
    ui->plainTextEdit->clear();
    if(!arg1){
        std::cout<<"更改为单次模拟模式"<<std::endl;
        ready = false;
    }else{
        std::cout<<"更改为千界一乘模式"<<std::endl;
        ready = false;
    }
}
void MainWindow::seth(int num){
    hero *testh = herofac(num);
    testh->init(false);
    std::string name = testh->name;
    if(ready){
        h2num = num;
        std::cout<<std::endl<<"++++第二位英桀设置为"<<name<<"++++"<<std::endl;
        std::cout.flush();
        run();
    }else{
        ui->plainTextEdit->clear();
        std::cout<<std::endl<<"++++第一位英桀设置为"<<name<<"++++"<<std::endl;
        h1num = num;
    }
    ready = !ready;
}
void MainWindow::run(){
       if (qjyc)
        {
            int times = 1000000;
            // try
            // {
            // }
            // catch(const std::exception& e)
            // {
            //     std::cerr << e.what() << '\n';
            //     std::cerr << "输入不合法！"<< '\n';
            //     helper();
            //     return 1;
            // }
            std::cout << std::endl
                      << "千界一乘第零额定功率：";

                    int win_num1 = 0;
                    int win_num2 = 0;
                    for (int t = 0; t < times; t++)
                    {
                        hero *hr1 = herofac(h1num);
                        hero *hr2 = herofac(h2num);
                        hr1->init(true);
                        hr2->init(true);
                        int round = 1;
                        while (round < 30)
                        {
                            if (hr1->on_round(round))
                                break;
                            if (hr2->on_round(round))
                                break;
                            if (hr1->status.spd > hr2->status.spd)
                            {
                                if (hr1->attack(hr2, round))
                                    break;
                                if (hr2->attack(hr1, round))
                                    break;
                            }
                            else if(hr1->status.spd == hr2->status.spd){
                                if(u(e)<0.5){
                                    if (hr1->attack(hr2, round))
                                        break;
                                    if (hr2->attack(hr1, round))
                                        break;
                                }else{
                                    if (hr2->attack(hr1, round))
                                        break;
                                    if (hr1->attack(hr2, round))
                                        break;
                                }
                            }
                            else
                            {
                                if (hr2->attack(hr1, round))
                                    break;
                                if (hr1->attack(hr2, round))
                                    break;
                            }
                            round++;
                        }
                        if ((int)hr1->live)
                        {
                            win_num1++;
                        }
                        if ((int)hr2->live)
                        {
                            win_num2++;
                        }
                    }
                    hero *hr1 = herofac(h1num);
                    hero *hr2 = herofac(h2num);
                    hr1->init(false);
                    hr2->init(false);
                    std::cout << std::endl
                              << "对战双方 " << hr1->name << " VS " << hr2->name;
                    std::cout << std::endl<<"    "
                              << hr1->name << "获胜" << win_num1 << "次，胜率" << (float)win_num1 / times;
                    std::cout << std::endl<<"    "
                              << hr2->name << "获胜" << win_num2 << "次，胜率" << (float)win_num2 / times;
//                    std::cout.flush();
       }
        else
        {
            hero *hr1 = herofac(h1num);
            hero *hr2 = herofac(h2num);
            hr1->init(false);
            hr2->init(false);
            int round = 1;
            std::cout << std::endl
                      << "对战双方 " << hr1->name << " VS " << hr2->name;
            while (round < 100)
            {
                std::cout << std::endl
                          << "=========round " << round;
                if (hr1->on_round(round))
                    break;
                if (hr2->on_round(round))
                    break;
                if (hr1->status.spd > hr2->status.spd)
                {
                    if (hr1->attack(hr2, round))
                        break;
                    if (hr2->attack(hr1, round))
                        break;
                }
                else if(hr1->status.spd == hr2->status.spd){
                    if(u(e)<0.5){
                        if (hr1->attack(hr2, round))
                            break;
                        if (hr2->attack(hr1, round))
                            break;
                    }else{
                        if (hr2->attack(hr1, round))
                            break;
                        if (hr1->attack(hr2, round))
                            break;
                    }
                }
                else
                {
                    if (hr2->attack(hr1, round))
                        break;
                    if (hr1->attack(hr2, round))
                        break;
                }
                round++;
                std::cout << std::endl
                          << "剩余血量 " << hr1->name << "：" << (int)hr1->status.hp << " " << hr2->name << "：" << (int)hr2->status.hp;
            }
            if ((int)hr1->live)
            {
                std::cout << std::endl
                          << hr1->name << "获胜！" << std::endl;
            }
            if ((int)hr2->live)
            {
                std::cout << std::endl
                          << hr2->name << "获胜！" << std::endl;
            }
        }
       std::cout.flush();
}

void MainWindow::on_pushButton_11_released()
{
seth(11);
}

void MainWindow::on_pushButton_4_released()
{
seth(4);
}

void MainWindow::on_pushButton_released()
{
seth(1);
}



void MainWindow::on_pushButton_3_released()
{
seth(3);
}

void MainWindow::on_pushButton_9_released()
{
    seth(9);
}

void MainWindow::on_pushButton_8_released()
{
    seth(8);
}

void MainWindow::on_pushButton_5_released()
{
    seth(5);
}

void MainWindow::on_pushButton_7_released()
{
    seth(7);
}

void MainWindow::on_pushButton_10_released()
{
    seth(10);
}

void MainWindow::on_pushButton_2_released()
{
    seth(2);
}

void MainWindow::on_pushButton_6_released()
{
    seth(6);
}

void MainWindow::on_pushButton_12_released()
{
    seth(12);
}


