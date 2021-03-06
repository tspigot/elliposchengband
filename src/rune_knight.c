/****************************************************************
 * The Rune Knight
 ****************************************************************/

#include "angband.h"

/****************************************************************
 * Public Helpers
 ****************************************************************/
void rune_calc_bonuses(object_type *o_ptr)
{
    if (o_ptr->rune == RUNE_ABSORPTION)
        p_ptr->magic_resistance += 15;
    if (o_ptr->rune == RUNE_UNDERSTANDING && object_is_helmet(o_ptr))
        p_ptr->auto_pseudo_id = TRUE;
    if (o_ptr->rune == RUNE_SHADOW)
    {
        if (object_is_body_armour(o_ptr) || o_ptr->tval == TV_CLOAK)
            p_ptr->skills.stl += 5 * p_ptr->lev / 50;
    }
    if (o_ptr->rune == RUNE_HASTE)
    {
        if (o_ptr->tval == TV_BOOTS)
            p_ptr->pspeed += 3 * p_ptr->lev / 50;
    }
}

void rune_calc_stats(object_type *o_ptr, s16b stats[MAX_STATS])
{
    if (o_ptr->rune == RUNE_UNDERSTANDING)
    {
        if (o_ptr->tval == TV_LITE)
            stats[A_INT] += 1;
        else
            stats[A_INT] += 2;
    }
    if (o_ptr->rune == RUNE_HASTE)
    {
        if (o_ptr->tval == TV_GLOVES)
            stats[A_DEX] += 2;
    }
    if (o_ptr->rune == RUNE_LIFE)
    {
        if (object_is_body_armour(o_ptr))
            stats[A_CON] += 1;
    }
    if (o_ptr->rune == RUNE_MIND)
    {
        if (object_is_helmet(o_ptr))
            stats[A_INT] += 2;
    }
    if (o_ptr->rune == RUNE_MIGHT)
    {
        stats[A_STR] += 2;
        stats[A_CON] += 2;
        if (object_is_body_armour(o_ptr))
            stats[A_DEX] += 2;
    }
}

cptr rune_desc(int which)
{
    switch (which)
    {
    case RUNE_ABSORPTION:
        return "{Absorption}";
    case RUNE_PROTECTION:
        return "{Protection}";
    case RUNE_REGENERATION:
        return "{Regeneration}";
    case RUNE_FIRE:
        return "{Fire}";
    case RUNE_AIR:
        return "{Air}";
    case RUNE_WATER:
        return "{Water}";
    case RUNE_LIGHT:
        return "{Light}";
    case RUNE_SHADOW:
        return "{Shadow}";
    case RUNE_EARTH:
        return "{Earth}";
    case RUNE_UNDERSTANDING:
        return "{Understanding}";
    case RUNE_ELEMENTAL_PROTECTION:
        return "{Preservation}";
    case RUNE_HASTE:
        return "{Haste}";
    case RUNE_SEEING:
        return "{Seeing}";
    case RUNE_SACRIFICE:
        return "{Sacrifice}";
    case RUNE_LIFE:
        return "{Life}";
    case RUNE_STABILITY:
        return "{Stability}";
    case RUNE_REFLECTION:
        return "{Reflection}";
    case RUNE_DEATH:
        return "{Death}";
    case RUNE_MIND:
        return "{Mind}";
    case RUNE_MIGHT:
        return "{Might}";
    case RUNE_DESTRUCTION:
        return "{Destruction}";
    case RUNE_GOOD_FORTUNE:
        return "{Luck}";
    case RUNE_IMMORTALITY:
        return "{Immortality}";
    }
    return "{Unknown}";
}

void _add_flag(obj_ptr obj, int which)
{
    add_flag(obj->flags, which);
    add_flag(obj->known_flags, which);
}

bool rune_add(object_type *o_ptr, int which, bool prompt)    /* Birthing needs access to this ... */
{
    char o_name[MAX_NLEN];

    if (!which) return FALSE;
    object_desc(o_name, o_ptr, 0);

    if (o_ptr->rune)
    {
        msg_format("%^s already has an attached rune.", o_name);
        return FALSE;
    }

    if (object_is_(o_ptr, TV_SWORD, SV_RUNESWORD))
    {
        msg_print("Failed! Rune Swords already have runes, albeit ones you fail to comprehend.");
        return FALSE;
    }

    if (o_ptr->number > 1)
    {
        msg_print("Failed! You may only add a rune to a single object at a time.");
        return FALSE;
    }

    if (prompt)
    {
        if (!get_check(
                format("Really add %^s to %s?",
                    rune_desc(which), o_name))) return FALSE;
    }

    o_ptr->rune = which;
    if (object_is_nameless(o_ptr))
        o_ptr->discount = 99;

    /* Note: Any effect that requires a pval will need to be handled
       silently in calc_bonuses(). This is because we keep the pval
       of the original object (e.g. Crown of Might (+3) <<Might>>
       Gives +5 Str/Con and +3 Dex, where the Rune adds +2 Str/Con) */
    switch (which)
    {
    case RUNE_PROTECTION:
        _add_flag(o_ptr, OF_IGNORE_ACID);
        o_ptr->to_a += 2 + randint1(8);
        break;

    case RUNE_REGENERATION:
        _add_flag(o_ptr, OF_REGEN);
        break;

    case RUNE_FIRE:
        if (object_is_melee_weapon(o_ptr) || o_ptr->tval == TV_GLOVES)
            _add_flag(o_ptr, OF_BRAND_FIRE);
        if (object_is_shield(o_ptr))
            _add_flag(o_ptr, OF_RES_FIRE);
        if (object_is_body_armour(o_ptr))
        {
            _add_flag(o_ptr, OF_RES_FIRE);
            _add_flag(o_ptr, OF_AURA_FIRE);
        }
        if (o_ptr->tval == TV_LITE || o_ptr->tval == TV_CLOAK)
            _add_flag(o_ptr, OF_AURA_FIRE);
        break;

    case RUNE_AIR:
        if (!object_is_melee_weapon(o_ptr))
            _add_flag(o_ptr, OF_LEVITATION);
        break;

    case RUNE_WATER:
        _add_flag(o_ptr, OF_IGNORE_ACID);
        if (object_is_melee_weapon(o_ptr) || o_ptr->tval == TV_GLOVES)
            _add_flag(o_ptr, OF_BRAND_ACID);
        else
            _add_flag(o_ptr, OF_RES_ACID);
        break;

    case RUNE_LIGHT:
        _add_flag(o_ptr, OF_RES_LITE);
        break;

    case RUNE_SHADOW:
        if (o_ptr->tval != TV_CLOAK)
            _add_flag(o_ptr, OF_RES_DARK);
        break;

    case RUNE_EARTH:
        if (object_is_melee_weapon(o_ptr))
            _add_flag(o_ptr, OF_VORPAL);
        else if (object_is_body_armour(o_ptr))
        {
            _add_flag(o_ptr, OF_RES_SHARDS);
            _add_flag(o_ptr, OF_AURA_SHARDS);
        }
        else if (object_is_shield(o_ptr))
            _add_flag(o_ptr, OF_RES_SHARDS);
        else if (o_ptr->tval == TV_CLOAK)
            _add_flag(o_ptr, OF_AURA_SHARDS);
        break;

    case RUNE_SEEING:
        _add_flag(o_ptr, OF_RES_BLIND);
        if (object_is_helmet(o_ptr))
            _add_flag(o_ptr, OF_SEE_INVIS);
        break;

    case RUNE_LIFE:
        _add_flag(o_ptr, OF_HOLD_LIFE);
        break;

    case RUNE_STABILITY:
        _add_flag(o_ptr, OF_RES_NEXUS);
        if (object_is_body_armour(o_ptr))
        {
            _add_flag(o_ptr, OF_RES_CHAOS);
            _add_flag(o_ptr, OF_RES_DISEN);
        }
        break;
    
    case RUNE_REFLECTION:
        _add_flag(o_ptr, OF_REFLECT);
        break;

    case RUNE_DEATH:
        if (object_is_melee_weapon(o_ptr))
            _add_flag(o_ptr, OF_BRAND_VAMP);
        else
        {
            _add_flag(o_ptr, OF_RES_NETHER);
            if (object_is_body_armour(o_ptr))
                _add_flag(o_ptr, OF_RES_POIS);
        }
        break;

    case RUNE_MIND:
        _add_flag(o_ptr, OF_TELEPATHY);
        if (object_is_helmet(o_ptr))
            _add_flag(o_ptr, OF_SUST_INT);
        break;

    case RUNE_MIGHT:
        o_ptr->to_h += randint1(5);
        o_ptr->to_d += randint1(5);
        if (object_is_body_armour(o_ptr))
        {
            _add_flag(o_ptr, OF_SUST_STR);
            _add_flag(o_ptr, OF_SUST_DEX);
            _add_flag(o_ptr, OF_SUST_CON);
        }
        break;

    case RUNE_DESTRUCTION:
        if (object_is_melee_weapon(o_ptr))
            o_ptr->dd += 2;
        else
        {
            o_ptr->to_h += 3 + randint1(8);
            o_ptr->to_d += 3 + randint1(8);
        }
        break;

    case RUNE_IMMORTALITY:
        _add_flag(o_ptr, OF_RES_TIME);
        if (object_is_body_armour(o_ptr))
        {
            _add_flag(o_ptr, OF_SUST_STR);
            _add_flag(o_ptr, OF_SUST_INT);
            _add_flag(o_ptr, OF_SUST_WIS);
            _add_flag(o_ptr, OF_SUST_DEX);
            _add_flag(o_ptr, OF_SUST_CON);
            _add_flag(o_ptr, OF_SUST_CHR);
            _add_flag(o_ptr, OF_HOLD_LIFE);
        }
        break;

    case RUNE_ELEMENTAL_PROTECTION:
    case RUNE_GOOD_FORTUNE:
        _add_flag(o_ptr, OF_IGNORE_ACID);
        _add_flag(o_ptr, OF_IGNORE_FIRE);
        _add_flag(o_ptr, OF_IGNORE_COLD);
        _add_flag(o_ptr, OF_IGNORE_ELEC);
        break;
    }

    if (prompt)
        msg_format("%^s gleams.", o_name);
    p_ptr->update |= PU_BONUS;

    return TRUE;
}

/****************************************************************
 * Private Spells and Helpers
 ****************************************************************/
static object_type *_rune_object_prompt(obj_p filter)
{
    obj_prompt_t prompt = {0};

    prompt.prompt = "Enchant which item?";
    prompt.error = "You have nothing to enchant.";
    prompt.filter = filter;
    prompt.where[0] = INV_PACK;
    prompt.where[1] = INV_EQUIP;
    prompt.where[2] = INV_FLOOR;

    obj_prompt(&prompt);
    return prompt.obj;
}

static void _rune_default_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_COST_EXTRA:
        var_set_int(res, MAX(25, p_ptr->msp));
        break;
    /*case SPELL_COLOR:
        var_set_int(res, TERM_L_BLUE);
        break; */
    default:
        default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_absorption_pred(object_type *o_ptr)
{
    if ( object_is_body_armour(o_ptr)
      || object_is_melee_weapon(o_ptr)
      || object_is_shield(o_ptr) )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_absorption_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Absorption");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Absorption on chosen melee weapon, body armor or shield. This rune grants a special magical defense that absorbs damage from all monster spells restoring your mana in the process.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_absorption_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_ABSORPTION, TRUE));

        break;
    }
    case SPELL_COST_EXTRA:
        var_set_int(res, 0); /* was MAX(1, p_ptr->msp * 3 / 10)*/
        break;
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_protection_pred(object_type *o_ptr)
{
    if (object_is_armour(o_ptr))
        return TRUE;
    return FALSE;
}
static void _rune_of_protection_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Protection");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Protection on chosen armor making it immune to acid while slightly enhancing its protective capabilities.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_protection_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_PROTECTION, TRUE));
        
        break;
    }
    case SPELL_COST_EXTRA:
        var_set_int(res, MAX(5, p_ptr->msp * 5 / 10));
        break;
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_regeneration_pred(object_type *o_ptr)
{
    if ( object_is_body_armour(o_ptr)
      || o_ptr->tval == TV_CLOAK )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_regeneration_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Regeneration");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Regeneration on chosen item granting powers of regeneration.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_regeneration_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_REGENERATION, TRUE));
        
        break;
    }
    case SPELL_COST_EXTRA:
        var_set_int(res, MAX(10, p_ptr->msp * 5 / 10));
        break;
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_fire_pred(object_type *o_ptr)
{
    if ( object_is_body_armour(o_ptr)
      || object_is_melee_weapon(o_ptr)
      || object_is_shield(o_ptr) 
      || o_ptr->tval == TV_CLOAK
      || (o_ptr->tval == TV_GLOVES && p_ptr->lev >= 45)
      || o_ptr->tval == TV_LITE )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_fire_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Fire");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Fire on chosen item granting special fire based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_fire_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_FIRE, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_air_pred(object_type *o_ptr)
{
    if ( (object_is_melee_weapon(o_ptr) && p_ptr->lev >= 40)
      || o_ptr->tval == TV_CLOAK
      || o_ptr->tval == TV_BOOTS )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_air_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Air");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Air on chosen item granting special wind based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_air_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_AIR, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_water_pred(object_type *o_ptr)
{
    if ( object_is_body_armour(o_ptr)
      || object_is_melee_weapon(o_ptr)
      || object_is_shield(o_ptr) 
      || o_ptr->tval == TV_CLOAK
      || (o_ptr->tval == TV_GLOVES && p_ptr->lev >= 45) )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_water_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Water");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Water on chosen item granting special acid based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_water_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_WATER, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_light_pred(object_type *o_ptr)
{
    if ( object_is_helmet(o_ptr)
      || o_ptr->tval == TV_LITE )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_light_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Light");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Light on chosen item granting special light based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_light_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_LIGHT, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_shadow_pred(object_type *o_ptr)
{
    if ( object_is_shield(o_ptr)
      || object_is_body_armour(o_ptr)
      || object_is_helmet(o_ptr)
      || o_ptr->tval == TV_CLOAK )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_shadow_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Shadow");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Shadow on chosen item granting special darkness based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_shadow_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_SHADOW, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_earth_pred(object_type *o_ptr)
{
    if ( (object_is_melee_weapon(o_ptr) && p_ptr->lev >= 35)
      || object_is_shield(o_ptr)
      || object_is_body_armour(o_ptr)
      || o_ptr->tval == TV_CLOAK )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_earth_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Earth");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Earth on chosen item granting special shard based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_earth_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_EARTH, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_understanding_pred(object_type *o_ptr)
{
    if ( object_is_helmet(o_ptr)
      || o_ptr->tval == TV_LITE )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_understanding_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Understanding");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Understanding on chosen item granting special knowledge based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_understanding_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_UNDERSTANDING, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static void _rune_of_elemental_protection_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Preservation");
        break;
    case SPELL_DESC:
        var_set_string(res, "Creates a standalone rune. As long as you have this rune in your inventory, your inventory items are less likely to be destroyed by elemental attacks.");
        break;
    case SPELL_CAST:
    {
        object_type forge;

        object_prep(&forge, lookup_kind(TV_RUNE, SV_RUNE));
        rune_add(&forge, RUNE_ELEMENTAL_PROTECTION, FALSE);
        drop_near(&forge, -1, py, px);

        var_set_bool(res, TRUE);
        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_haste_pred(object_type *o_ptr)
{
    if ( o_ptr->tval == TV_GLOVES
      || o_ptr->tval == TV_BOOTS )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_haste_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Haste");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Haste on chosen item granting special speed based powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_haste_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_HASTE, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_seeing_pred(object_type *o_ptr)
{
    if ( object_is_helmet(o_ptr)
      || o_ptr->tval == TV_LITE )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_seeing_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Seeing");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Seeing on chosen item granting powers of sight.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_seeing_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_SEEING, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static void _rune_of_sacrifice_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Sacrifice");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Sacrifice on an artifact. You can now destroy (with 'k' command) the artifact, and if you do so, you restore HP and SP.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(object_is_artifact);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_SACRIFICE, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_life_pred(object_type *o_ptr)
{
    if ( object_is_shield(o_ptr)
      || object_is_body_armour(o_ptr)
      || o_ptr->tval == TV_LITE )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_life_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Life");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Life on chosen item protecting your living essence.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_life_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_LIFE, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_stability_pred(object_type *o_ptr)
{
    if ( object_is_body_armour(o_ptr)
      || object_is_helmet(o_ptr)
      || o_ptr->tval == TV_CLOAK
      || o_ptr->tval == TV_BOOTS )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_stability_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Stability");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Stability on chosen item protecting you from the vicissitudes of the world around you.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_stability_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_STABILITY, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static void _rune_of_reflection_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Reflection");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Reflection on your shield. Your shield gains the Reflection property.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(object_is_shield);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_REFLECTION, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_death_pred(object_type *o_ptr)
{
    if ( object_is_melee_weapon(o_ptr)
      || object_is_shield(o_ptr)
      || object_is_body_armour(o_ptr)
      || object_is_helmet(o_ptr) )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_death_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Death");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Death on chosen item granting powers of the nether world.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_death_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_DEATH, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_mind_pred(object_type *o_ptr)
{
    if ( object_is_helmet(o_ptr) 
      || o_ptr->tval == TV_LITE ) 
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_mind_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Mind");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Mind on chosen item granting powers of thought and awareness.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_mind_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_MIND, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_might_pred(object_type *o_ptr)
{
    if ( object_is_body_armour(o_ptr) 
      || object_is_helmet(o_ptr) ) 
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_might_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Might");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Might on chosen item granting powers of strength and fortitude.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_might_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_MIGHT, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_destruction_pred(object_type *o_ptr)
{
    if ( object_is_melee_weapon(o_ptr) 
      || o_ptr->tval == TV_GLOVES ) 
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_destruction_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Destruction");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Destruction on chosen item granting great combat powers.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_destruction_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_DESTRUCTION, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static void _rune_of_good_fortune_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Luck");
        break;
    case SPELL_DESC:
        var_set_string(res, "Creates a standalone rune. As long as you have this rune in your inventory you will experience better luck during your adventures.");
        break;
    case SPELL_CAST:
    {
        object_type forge;

        object_prep(&forge, lookup_kind(TV_RUNE, SV_RUNE));
        rune_add(&forge, RUNE_GOOD_FORTUNE, FALSE);
        drop_near(&forge, -1, py, px);

        var_set_bool(res, TRUE);
        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

static bool _rune_of_immortality_pred(object_type *o_ptr)
{
    if ( object_is_shield(o_ptr) 
      || object_is_body_armour(o_ptr)
      || object_is_helmet(o_ptr) 
      || o_ptr->tval == TV_CLOAK )
    {
        return TRUE;
    }
    return FALSE;
}
static void _rune_of_immortality_spell(int cmd, variant *res)
{
    switch (cmd)
    {
    case SPELL_NAME:
        var_set_string(res, "Rune of Immortality");
        break;
    case SPELL_DESC:
        var_set_string(res, "Places a Rune of Immortality on chosen item granting power over time itself.");
        break;
    case SPELL_CAST:
    {
        object_type *o_ptr = _rune_object_prompt(_rune_of_immortality_pred);
        var_set_bool(res, FALSE);

        if (o_ptr)
            var_set_bool(res, rune_add(o_ptr, RUNE_IMMORTALITY, TRUE));

        break;
    }
    default:
        _rune_default_spell(cmd, res);
        break;
    }
}

/****************************************************************
 * Spell Table and Exports
 ****************************************************************/
 #define _MAX_SPELLS_PER_GROUP    25
 #define _MAX_SPELL_GROUPS       4

typedef struct {
    cptr name;
    cptr help;
    int color;
    spell_info spells[_MAX_SPELLS_PER_GROUP];    /* There is always a sentinel at the end */
} _spell_group, *_spell_group_ptr;

static _spell_group _spell_groups[_MAX_SPELL_GROUPS] = {
    { "Runes of Creation",
      "Augment your equipment by attaching runes of various powers. Also, you may create "
      "certain stand alone runes that grant powers by virtue of being present in your "
      "inventory. Be sure to always keep Absorption handy, for it is your only means "
      "of regaining spell points!",
      TERM_L_BLUE,
      { {  1,   0, 0, _rune_of_absorption_spell },
        {  5,   0, 0, _rune_of_protection_spell },
        {  7,   0, 0, _rune_of_regeneration_spell },
        {  9,   0, 0, _rune_of_fire_spell },
        { 11,   0, 0, _rune_of_air_spell },
        { 13,   0, 0, _rune_of_water_spell },
        { 15,   0, 0, _rune_of_light_spell },
        { 17,   0, 0, _rune_of_shadow_spell },
        { 19,   0, 0, _rune_of_earth_spell },
        { 21,   0, 0, _rune_of_understanding_spell },
        { 23,   0, 0, _rune_of_elemental_protection_spell },
        { 25,   0, 0, _rune_of_haste_spell },
        { 27,   0, 0, _rune_of_seeing_spell },
        { 29,   0, 0, _rune_of_sacrifice_spell },
        { 31,   0, 0, _rune_of_life_spell },
        { 32,   0, 0, _rune_of_stability_spell },
        { 33,   0, 0, _rune_of_reflection_spell },
        { 35,   0, 0, _rune_of_death_spell },
        { 37,   0, 0, _rune_of_mind_spell },
        { 39,   0, 0, _rune_of_might_spell },
        { 41,   0, 0, _rune_of_destruction_spell },
        { 43,   0, 0, _rune_of_good_fortune_spell },
        { 45,   0, 0, _rune_of_immortality_spell },
        { -1,   0,  0, NULL },
      }
    },
    { "Runes of the Novice",
      "Minor spells and powers, available to the weakest of Rune-Knights. "
      "While hardly awe-inspiring, these powers grant detection and weak "
      "utility that are designed to assist the novice in their quest for deeper "
      "understanding.",
      TERM_L_GREEN,
      { {  1,   1, 20, detect_monsters_spell },
        {  1,   2, 25, phase_door_spell },
        {  3,   3, 25, detect_doors_stairs_traps_spell },
        {  5,   5, 35, light_area_spell },
        {  7,  10, 75, resist_poison_spell },
        {  9,   7, 75, magic_mapping_spell },
        { 11,   9, 35, summon_manes_spell },
        { 12,  12, 40, orb_of_entropy_spell },
        { 15,   9, 35, teleport_spell },
        { -1,   0,  0, NULL },
      }
    },
    { "Runes of the Initiate",
      "Stronger rune powers, available to the experienced Rune-Knight. "
      "These powers offer great utility to assist you on your journey "
      "of knowledge.",
      TERM_UMBER,
      { { 16,  16, 45, remove_curse_I_spell },
        { 18,  12, 60, teleport_other_spell },
        { 20,  20, 85, satisfy_hunger_spell },
        { 21,  20, 80, explosive_rune_spell },
        { 22,  16, 60, stone_to_mud_spell },
        { 25,  25, 85, disintegrate_spell },
        { 28,  20, 70, resistance_spell },
        { 29,  23, 60, protection_from_evil_spell },
        { 30,  25, 75, battle_frenzy_spell },
        { -1,   0,  0, NULL },
      }
    },
    { "Runes of the Master",
      "Mighty powers indeed. Use them wisely, for the forces of evil have "
      "grown strong and don't take well to rivals in their quest for domination.",
      TERM_RED,
      { { 33,  30, 75, identify_fully_spell },
        { 35,  33, 45, dimension_door_spell },
        { 36,  70, 75, glyph_of_warding_spell },
        { 38,  65, 85, mana_branding_spell },
        { 40,  40, 80, eye_for_an_eye_spell },
        { 42, 100, 80, clairvoyance_spell },
        { 43, 100, 45, living_trump_spell },
        { 45,  58, 85, mana_storm_II_spell },
        { 47, 100, 90, wraithform_spell },
        { 49,  80, 85, polymorph_demonlord_spell },
        { -1,   0,  0, NULL },
      }
    },
};

static int _get_spells_imp(spell_info* spells, int max, _spell_group *spell_group)
{
    int i;
    int ct = 0;
    int stat_idx = p_ptr->stat_ind[A_INT];
    
    for (i = 0; ; i++)
    {
        spell_info *base = &spell_group->spells[i];
        if (base->level < 0) break;
        if (ct >= max) break;
        if (base->level <= p_ptr->lev)
        {
            spell_info* current = &spells[ct];
            current->fn = base->fn;
            current->level = base->level;
            current->cost = base->cost;

            current->fail = calculate_fail_rate(base->level, base->fail, stat_idx);            
            ct++;
        }
    }
    return ct;
}

static void _character_dump(doc_ptr doc)
{
    int i;
    doc_printf(doc, "<topic:Spells>==================================== <color:keypress>S</color>pells ===================================\n\n");
    for (i = 0; i < _MAX_SPELL_GROUPS; i++)
    {
        _spell_group_ptr group = &_spell_groups[i];
        spell_info       spells[_MAX_SPELLS_PER_GROUP];
        int              ct = _get_spells_imp(spells, _MAX_SPELLS_PER_GROUP, group); 

        if (!ct) continue;
        doc_printf(doc, "<color:%c>%s</color>\n", attr_to_attr_char(group->color), group->name);
        py_display_spells_aux(doc, spells, ct);
    }
}

static void _spell_menu_fn(int cmd, int which, vptr cookie, variant *res)
{
    switch (cmd)
    {
    case MENU_TEXT:
        var_set_string(res, _spell_groups[which].name);
        break;
    case MENU_HELP:
        var_set_string(res, _spell_groups[which].help);
        break;
    case MENU_COLOR:
        var_set_int(res, _spell_groups[which].color);
        break;
    default:
        default_menu(cmd, which, cookie, res);
    }
}

static int _get_spells(spell_info* spells, int max)
{
    int idx = -1;
    int ct = 0;
    menu_t menu = { "Use which group of spells?", "Browse which group of spells?", NULL,
                    _spell_menu_fn, _spell_groups, _MAX_SPELL_GROUPS};

    idx = menu_choose(&menu);
    if (idx < 0) return 0;
    ct = _get_spells_imp(spells, max, &_spell_groups[idx]);
    if (ct == 0)
        msg_print("You don't know any of those spells yet!");
    return ct;
}

static void _calc_bonuses(void)
{
    p_ptr->spell_cap += 7;
}

static caster_info * _caster_info(void)
{
    static caster_info me = {0};
    static bool init = FALSE;
    if (!init)
    {
        me.magic_desc = "spell";
        me.which_stat = A_INT;
        me.encumbrance.max_wgt = 450;
        me.encumbrance.weapon_pct = 20;
        me.encumbrance.enc_wgt = 1200;
        init = TRUE;
    }
    return &me;
}

static void _birth(void)
{
    object_type forge = {0};
    object_prep(&forge, lookup_kind(TV_SWORD, SV_BROAD_SWORD));
    rune_add(&forge, RUNE_FIRE, FALSE);
    py_birth_obj(&forge);

    py_birth_obj_aux(TV_SOFT_ARMOR, SV_SOFT_LEATHER_ARMOR, 1);
    py_birth_obj_aux(TV_POTION, SV_POTION_SPEED, 1);
}

static bool _destroy_object(obj_ptr obj)
{
    if (obj->rune == RUNE_SACRIFICE)
    {
        bool is_equipped = obj->loc.where == INV_EQUIP;
        int add_hp = is_equipped ? p_ptr->mhp : p_ptr->mhp/3;
        int add_sp = is_equipped ? p_ptr->msp : p_ptr->msp/3;

        msg_print("You feel a surge of wondrous power enter your body.");

        p_ptr->chp = MIN(p_ptr->mhp, p_ptr->chp + add_hp);
        p_ptr->chp_frac = 0;
        p_ptr->csp = MIN(p_ptr->msp, p_ptr->csp + add_sp);
        p_ptr->csp_frac = 0;

        p_ptr->redraw |= (PR_MANA);
        p_ptr->window |= (PW_SPELL);
        p_ptr->redraw |= (PR_HP);

        if (is_equipped)
        {
            blast_object(obj);
            obj->curse_flags = OFC_HEAVY_CURSE;
        }
        return TRUE;
    }
    return FALSE;
}

class_t *rune_knight_get_class(void)
{
    static class_t me = {0};
    static bool init = FALSE;

    if (!init)
    {           /* dis, dev, sav, stl, srh, fos, thn, thb */
    skills_t bs = { 30,  35,  36,   2,  18,  16,  50,  25};
    skills_t xs = {  7,  10,  10,   0,   0,   0,  15,  11};

        me.name = "Rune-Knight";
        me.desc = "The Rune Knight is a mythical warrior-mage who is dedicated to the power "
                  "of ancient Runes that hold immense power. By affixing these Runes to his "
                  "equipment, the Rune Knight can become an avatar of destruction, or an "
                  "invulnerable bastion. Unlike the Warrior-Mage and all other casters, the "
                  "Rune Knight's mana does not regenerate on its own; rather, the Rune Knight "
                  "must siphon mana from magical attacks directed at him. The Rune Knight has "
                  "a fixed (though very large) selection of spells that he can use his mana on, "
                  "in addition to his unique Rune spells.";

        me.stats[A_STR] = 2;
        me.stats[A_INT] = 2;
        me.stats[A_WIS] = 0;
        me.stats[A_DEX] = 1;
        me.stats[A_CON] = 0;
        me.stats[A_CHR] = 1;
        me.base_skills = bs;
        me.extra_skills = xs;
        me.life = 107;
        me.base_hp = 6;
        me.exp = 150;
        me.pets = 35;
        me.flags = CLASS_SENSE1_FAST | CLASS_SENSE1_STRONG;

        me.birth = _birth;
        me.calc_bonuses = _calc_bonuses;
        me.caster_info = _caster_info;
        me.get_spells = _get_spells;
        me.destroy_object = _destroy_object;
        me.character_dump = _character_dump;
        init = TRUE;
    }

    return &me;
}
