/*
 *    raw_polymorphism_c.c
 *    implements raw polymorphism in C.
 *
 *    It's based on the idea that sometimes we have a internal codebase
 *    that is not to be changed, or even just re-compiled, but we want 
 *    to add new data types that it can handle.
 *
 *    I did it in C because it's more simple to set up a scene where you
 *    must implement the polymorphism by yourself.
 *
 */


#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

/* begin(result types) */

typedef
    struct drink_result
    {
        bool success;
        const char *message;
    }
    drink_result_t;

typedef
    struct attack_with_result
    {
        bool success;
        const char *message;
    }
    attack_with_result_t;

/* end(result types); begin(info table) */

typedef struct metadrink metadrink_t;
typedef struct metaarms metaarms_t;

typedef
    struct metainfos
    {
        metadrink_t *drink;
        metaarms_t *arms;
    }
    metainfos_t;

/* end(info table); begin(meta info types) */

struct metadrink
{
    drink_result_t (*addr_drink)(void *);
    ptrdiff_t offset_temperature;
};

int16_t get_temperature(void *self_ptr)
{
    return *(int16_t*)((char*)self_ptr + ((metainfos_t*)self_ptr)->drink->offset_temperature);
}

drink_result_t drink(void *self_ptr)
{
    if((((metainfos_t*)self_ptr)->drink) == nullptr)
    {
        exit(0);
    }
    
    return ((metainfos_t*)self_ptr)->drink->addr_drink(self_ptr);
}

struct metaarms
{
    attack_with_result_t (*addr_attack_with)(void *);
};

attack_with_result_t attack_with(void *self_ptr)
{
    if((((metainfos_t*)self_ptr)->arms) == nullptr)
    {
        exit(0);
    }
    
    return ((metainfos_t*)self_ptr)->arms->addr_attack_with(self_ptr);
}

/* end(meta info types), begin(test types) */

// struct 'water'
typedef
    struct water
    {
        metainfos_t meta;
        int16_t temperature;
    }
    water_t;

drink_result_t drink_water(void *self_ptr)
{
    auto self = (water_t *)self_ptr;
    if (self->temperature < 0)
    {
        return (drink_result_t){ .success = false, .message = "The water is frozen." };
    }
    else
    {
        return (drink_result_t){ .success = true, .message = nullptr };
    }
}

water_t* alloc_water(int16_t temperature)
{
    auto self_ptr = (water_t*) malloc(sizeof(water_t) + sizeof(metadrink_t));
    self_ptr->meta.drink = (metadrink_t*)((char*)self_ptr + sizeof(water_t));
    self_ptr->meta.drink->addr_drink = drink_water;
    self_ptr->meta.drink->offset_temperature = offsetof(water_t, temperature);
    self_ptr->temperature = temperature;
    self_ptr->meta.arms = nullptr;
    return self_ptr;
}

void free_water(water_t *self_ptr)
{
    free(self_ptr);
}

// struct 'coffee'
typedef
    struct coffee
    {
        metainfos_t meta;
        int16_t temperature;
        bool sugar;
    }
    coffee_t;

drink_result_t drink_coffee(void *self_ptr)
{
    coffee_t *self = (coffee_t *)self_ptr;
    if (self->temperature < 100)
    {
        return (drink_result_t){ .success = false, .message = "You only drink HOT coffee." };
    }
    else if (self->sugar)
    {
        return (drink_result_t){ .success = false, .message = "You don't drink coffee with sugar." };
    }
    else
    {
        return (drink_result_t){ .success = true, .message = nullptr };
    }
}

coffee_t* alloc_coffee(int16_t temperature, bool sugar)
{
    auto self_ptr = (coffee_t*) malloc(sizeof(coffee_t) + sizeof(metadrink_t));
    self_ptr->meta.drink = (metadrink_t*)((char*)self_ptr + sizeof(coffee_t));
    self_ptr->meta.drink->addr_drink = drink_coffee;
    self_ptr->meta.drink->offset_temperature = offsetof(coffee_t, temperature);
    self_ptr->meta.arms = nullptr;
    self_ptr->temperature = temperature;
    self_ptr->sugar = sugar;
    return self_ptr;
}

void free_coffee(coffee_t *self_ptr)
{
    free(self_ptr);
}

// struct 'sword'
typedef
    struct sword
    {
        metainfos_t meta;
        bool rusty;
    }
    sword_t;

attack_with_result_t attack_with_sword(void *self_ptr)
{
    sword_t *self = (sword_t *)self_ptr;
    if (self->rusty)
    {
        return (attack_with_result_t){ .success = false, .message = "The sword is rusty." };
    }
    else
    {
        return (attack_with_result_t){ .success = true, .message = nullptr };
    }
}

sword_t* alloc_sword(bool rusty)
{
    auto self_ptr = (sword_t*) malloc(sizeof(sword_t) + sizeof(metaarms_t));
    self_ptr->meta.arms = (metaarms_t*)((char*)self_ptr + sizeof(sword_t));
    self_ptr->meta.arms->addr_attack_with = attack_with_sword;
    self_ptr->meta.drink = nullptr;
    self_ptr->rusty = rusty;
    return self_ptr;
}

void free_sword(sword_t *self_ptr)
{
    free(self_ptr);
}

// struct 'poison'
typedef
    struct poison
    {
        metainfos_t meta;
        int16_t temperature;
    }
    poison_t;

drink_result_t drink_poison(void *self_ptr)
{
    return (drink_result_t){ .success = false, .message = "You don't drink poison." };
}

attack_with_result_t attack_with_poison(void *self_ptr)
{
    return (attack_with_result_t){ .success = true, .message = nullptr };
}

poison_t* alloc_poison(int16_t temperature)
{
    auto self_ptr = (poison_t*) malloc(sizeof(poison_t) + sizeof(metadrink_t) + sizeof(metaarms_t));
    self_ptr->meta.drink = (metadrink_t*)((char*)self_ptr + sizeof(poison_t));
    self_ptr->meta.drink->addr_drink = drink_poison;
    self_ptr->meta.drink->offset_temperature = offsetof(poison_t, temperature);
    self_ptr->meta.arms = (metaarms_t*)((char*)self_ptr + sizeof(poison_t) + sizeof(metadrink_t));
    self_ptr->meta.arms->addr_attack_with = attack_with_poison;
    self_ptr->temperature = temperature;
    return self_ptr;
}

void free_poison(poison_t *self_ptr)
{
    free(self_ptr);
}

/* end(test types) */

int main(void)
{
    auto w = alloc_water(20);
    auto c = alloc_coffee(90, false);
    auto s = alloc_sword(false);
    auto p = alloc_poison(-10);
    
    auto w_ret = drink(w);
    if (w_ret.success)
    {
        printf("You drink the water, which is %d degrees.\n", get_temperature(w));
    }
    else
    {
        printf("Error: %s\n", w_ret.message);
    }

    auto c_ret = drink(c);
    if (c_ret.success)
    {
        printf("You drink the coffee, which is %d degrees.\n", get_temperature(c));
    }
    else
    {
        printf("Error: %s\n", c_ret.message);
    }

    auto s_ret = attack_with(s);
    if (s_ret.success)
    {
        printf("You attack with the sword.\n");
    }
    else
    {
        printf("Error: %s\n", s_ret.message);
    }

    auto p_drink_ret = drink(p);
    if (p_drink_ret.success)
    {
        printf("You drink the poison, which is %d degrees.\n", get_temperature(p));
    }
    else
    {
        printf("Error: %s\n", p_drink_ret.message);
    }
    auto p_attack_ret = attack_with(p);
    if (p_attack_ret.success)
    {
        printf("You attack with the poison.\n");
    }
    else
    {
        printf("Error: %s\n", p_attack_ret.message);
    }

    free_water(w);
    free_coffee(c);
    free_sword(s);
    free_poison(p);
    return 0;
}