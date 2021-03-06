#include <gb/gb.h>
#include <rand.h>
#include <string.h>
#include "Game_Definitions.h"
#include "Game_Functions.h"
#include "Game_Map.h"
#include "Game_Enemy.h"
#include "Object_Pools.c"
#include "Tiles/Font_Menu.c"
#include "Tiles/Tileset_1.c"

Point room_queue[20];

UBYTE temp_x = 0, temp_y = 0;

const unsigned char temp_room[1] = {0x01};
const unsigned char temp_char_1[1] = {0x02};

GameCharacter* Find_Free_Enemy()
{
    if(!char_enemy_1.active)
    {
        return &char_enemy_1;
    }
    else if(!char_enemy_2.active)
    {
        return &char_enemy_2;
    }
    else if(!char_enemy_3.active)
    {
        return &char_enemy_3;
    }
    else if(!char_enemy_4.active)
    {
        return &char_enemy_4;
    }
    else
    {
        return NULL;
    }
}

void Load_Enemy(const GameEnemy* enemy, UBYTE pos_x, UBYTE pos_y)
{
    temp_char = Find_Free_Enemy();

    if(temp_char == NULL){return;}

    temp_char->active = true;
    temp_char->object.pos_x = pos_x;
    temp_char->object.pos_y = pos_y;
    temp_char->object.floating = enemy->floating;
    temp_char->health = enemy->health;
    temp_char->max_health = enemy->health;
    temp_char->ai_tick = 0;
    temp_char->object.on_collision = enemy->enemy_col;
    temp_char->update = enemy->enemy_ai;

    set_anim_package(temp_char, enemy->anims);
    Call_Load_Sprite(&temp_char->object, enemy->anims->char_down->sprites[0]);

    build_object(&temp_char->object);
    move_object(&temp_char->object, temp_char->object.pos_x, temp_char->object.pos_y);
}

void Load_Room_Enemies(const GameMap* map)
{
    for(u_i = 0; u_i < 5; u_i++)
    {
        if(map->troops[u_i].enemy == NULL){return;}

        Load_Enemy(map->troops[u_i].enemy, map->troops[u_i].pos.x, map->troops[u_i].pos.y);
    }
}

void Clear_Enemy(GameCharacter* character)
{
    character->active = false;
    character->object.pos_x = 0;
    character->object.pos_y = 0;
    character->object.velocity_x = 0;
    character->object.velocity_y = 0;
    character->object.floating = false;
    character->object.on_collision = NULL;
    character->facing = down;
    character->health = 0;
    character->max_health = 0;
    character->anim_frame = 0;
    character->anim_tick = 0;
    character->current_anim = NULL;
    character->damage_timer = 0;
    character->anims = NULL;
    character->ai_tick = 0;
    character->update = NULL;
    
    Call_Load_Sprite(&character->object, &sprite_clear);
}

void Count_Doors(UBYTE room_pos_x, UBYTE room_pos_y)
{
    door_count[up] = false;
    door_count[down] = false;
    door_count[left] = false;
    door_count[right] = false;

    for(u_x = 0; u_x < 4; u_x++)
    {
        map_x = room_pos_x;
        map_y = room_pos_y;

        switch(u_x)
        {
            case 0: map_y--; break;
            case 1: map_x++; break;
            case 2: map_y++; break;
            case 3: map_x--; break;
        }

        if(floor_map[map_y][map_x] != NULL)
        {
            switch(u_x)
            {
                case 0: door_count[up] = true; break;
                case 1: door_count[right] = true; break;
                case 2: door_count[down] = true; break;
                case 3: door_count[left] = true; break;
            }
        }
    }
}

const GameMap* Get_Map_From_Pool(UBYTE room_pos_x, UBYTE room_pos_y)
{
    Count_Doors(room_pos_x, room_pos_y);

    temp_UBYTE = rand() % 9;

    if(door_count[up] == true && door_count[down] == false && door_count[left] == false && door_count[right] == false) //* Top
    {
        return map_pool_t[0];
    }
    else if(door_count[up] == false && door_count[down] == true && door_count[left] == false && door_count[right] == false) //* Bottom
    {
        return map_pool_b[0];
    }
    else if(door_count[up] == false && door_count[down] == false && door_count[left] == true && door_count[right] == false) //* Left
    {
        return map_pool_l[0];
    }
    else if(door_count[up] == false && door_count[down] == false && door_count[left] == false && door_count[right] == true) //* Right
    {
        return map_pool_r[0];
    }
    else if(door_count[up] == true && door_count[down] == true && door_count[left] == false && door_count[right] == false) //* Top, Bottom
    {
        return map_pool_tb[0];
    }
    else if(door_count[up] == false && door_count[down] == false && door_count[left] == true && door_count[right] == true) //* Left, Right
    {
        return map_pool_lr[0];
    }
    else if(door_count[up] == true && door_count[down] == false && door_count[left] == true && door_count[right] == true) //* Top, Left, Right
    {
        return map_pool_tlr[0];
    }
    else if(door_count[up] == false && door_count[down] == true && door_count[left] == true && door_count[right] == true) //* Bottom, Left, Right
    {
        return map_pool_blr[0];
    }
    else if(door_count[up] == true && door_count[down] == true && door_count[left] == true && door_count[right] == false) //* Top, Bottom, Left
    {
        return map_pool_tbl[0];
    }
    else if(door_count[up] == true && door_count[down] == true && door_count[left] == false && door_count[right] == true) //* Top, Bottom, Right
    {
        return map_pool_tbr[0];
    }
    else if(door_count[up] == true && door_count[down] == false && door_count[left] == true && door_count[right] == false) //* Top, Left
    {
        return map_pool_tl[0];
    }
    else if(door_count[up] == true && door_count[down] == false && door_count[left] == false && door_count[right] == true) //* Top, Right
    {
        return map_pool_tr[0];
    }
    else if(door_count[up] == false && door_count[down] == true && door_count[left] == true && door_count[right] == false) //* Bottom, Left
    {
        return map_pool_bl[0];
    }
    else if(door_count[up] == false && door_count[down] == true && door_count[left] == false && door_count[right] == true) //* Bottom, Right
    {
        return map_pool_br[0];
    }
    else
    {
        return &map_dungeon_1;
    }
}

void Clear_Background()
{
    fill_bkg_rect(0, 0, 32, 32, 0);
}

void Load_Tileset(UBYTE tileset)
{
    Tileset = tileset;

    if(Tileset == 1)
    {
        set_bkg_data(0, 110, Tileset_1);
    }
    else
    {
        set_bkg_data(0, 110, Tileset_1);
    }
}

void Load_Map(const GameMap* map)
{
    current_map = map;

    Load_Tileset(map->tileset);

    Call_Draw_Map(map);
}

UBYTE Count_Neighbors(UBYTE pos_x, UBYTE pos_y)
{
    neighbors = 0;

    for(u_j = 0; u_j < 4; u_j++)
    {
        x = 0;
        y = 0;

        switch(u_j)
        {
            case 0: x = -1; break;
            case 1: y = -1; break;
            case 2: x = 1; break;
            case 3: y = 1; break;
        }

        temp_x = pos_x + x;
        temp_y = pos_y + y;

        if(floor_map[temp_y][temp_x] != NULL)
        {
            neighbors++;
        }
    }

    return neighbors;
}

void Generate_Floor()
{
    room_amount = 3 + rand() % 2;

    while(true)
    {
        for(u_i = 0; u_i < 11; u_i++) //* Reset 'floor_map'.
        {
            for(u_j = 0; u_j < 11; u_j++)
            {
                floor_map[u_i][u_j] = NULL;
            }
        }

        for(u_i = 0; u_i < 11; u_i++) //* Reset 'room_open'.
        {
            for(u_j = 0; u_j < 11; u_j++)
            {
                room_open[u_i][u_j] = false;
            }
        }

        for(u_i = 0; u_i < 20; u_i++) //* Reset 'room_queue'.
        {
            room_queue[u_i].x = 0;
            room_queue[u_i].y = 0;
        }

        for(u_i = 0; u_i < 15; u_i++) //* Reset 'end_rooms'.
        {
            end_rooms[u_i].x = 0;
            end_rooms[u_i].y = 0;
        }

        floor_map[5][5] = &map_dungeon_1;
        room_queue[0].x = 5;
        room_queue[0].y = 5;

        queue_pos = 0;
        room_count = 1;

        for(queue_amount = 0; queue_amount < 20; queue_amount++) //* Iterate through queued rooms.
        {
            load_pos_x = room_queue[queue_amount].x;
            load_pos_y = room_queue[queue_amount].y;

            for(u_i = 0; u_i < 4; u_i++) //* Generate rooms randomly north, south, east, and west of queued room.
            {
                x = 0;
                y = 0;

                switch(u_i)
                {
                    case 0: x = -1; break;
                    case 1: y = -1; break;
                    case 2: x = 1; break;
                    case 3: y = 1; break;
                }

                map_x = load_pos_x + x;
                map_y = load_pos_y + y;

                if(map_x > 0 && map_y > 0 && map_x < 10 && map_y < 10)
                {
                    if(floor_map[map_y][map_x] == NULL && Count_Neighbors(map_x, map_y) < 2 && room_count < room_amount && rand() % 10 < 5)
                    {
                        floor_map[map_y][map_x] = &map_dungeon_1;
                        room_queue[queue_pos].x = map_x;
                        room_queue[queue_pos].y = map_y;

                        queue_pos++;
                        room_count++;

                        last_room.x = map_x;
                        last_room.y = map_y;
                    }
                }

                if(room_count >= room_amount)
                {
                    break;
                }
            }

            if(room_count >= room_amount)
            {
                break;
            }
        }

        if(room_count >= room_amount)
        {
            break;
        }
    }

    for(u_j = 0; u_j < 11; u_j++)
    {
        for(u_i = 0; u_i < 11; u_i++)
        {
            if(floor_map[u_j][u_i] != NULL)
            {
                floor_map[u_j][u_i] = Get_Map_From_Pool(u_i, u_j);
            }
        }
    }

    ctr_end_room = 0;

    for(u_y = 0; u_y < 11; u_y++) //* Add all end rooms to 'end_rooms'
    {
        for(u_x = 0; u_x < 11; u_x++)
        {
            if(floor_map[u_y][u_x] != NULL)
            {
                if(Count_Neighbors(u_x, u_y) == 1 && floor_map[u_y][u_x]->type != room_boss)
                {
                    end_rooms[ctr_end_room].x = u_x;
                    end_rooms[ctr_end_room].y = u_y;

                    ctr_end_room++;
                }
            }
        }
    }

    u_i = rand() % ctr_end_room;

    if(floor_map[end_rooms[u_i].y][end_rooms[u_i].x]->doors[up] == true)
    {
        floor_map[end_rooms[u_i].y][end_rooms[u_i].x] = &map_dungeon_item_t;
    }
    else if(floor_map[end_rooms[u_i].y][end_rooms[u_i].x]->doors[down] == true)
    {
        floor_map[end_rooms[u_i].y][end_rooms[u_i].x] = &map_dungeon_item_b;
    }
    else if(floor_map[end_rooms[u_i].y][end_rooms[u_i].x]->doors[left] == true)
    {
        floor_map[end_rooms[u_i].y][end_rooms[u_i].x] = &map_dungeon_item_l;
    }
    else if(floor_map[end_rooms[u_i].y][end_rooms[u_i].x]->doors[right] == true)
    {
        floor_map[end_rooms[u_i].y][end_rooms[u_i].x] = &map_dungeon_item_r;
    }

    if(floor_map[last_room.y][last_room.x]->doors[up] == true)
    {
        floor_map[last_room.y][last_room.x] = &map_dungeon_boss_t;
    }
    else if(floor_map[last_room.y][last_room.x]->doors[down] == true)
    {
        floor_map[last_room.y][last_room.x] = &map_dungeon_boss_b;
    }
    else if(floor_map[last_room.y][last_room.x]->doors[left] == true)
    {
        floor_map[last_room.y][last_room.x] = &map_dungeon_boss_l;
    }
    else if(floor_map[last_room.y][last_room.x]->doors[right] == true)
    {
        floor_map[last_room.y][last_room.x] = &map_dungeon_boss_r;
    }
}

void Load_Floor()
{
    fade_out();

    Clear_Enemy(&char_enemy_1);
    Clear_Enemy(&char_enemy_2);
    Clear_Enemy(&char_enemy_3);
    Clear_Enemy(&char_enemy_4);

    move_bkg(0, 0);

    Clear_Background();

    Generate_Floor();

    Call_Load_Map(floor_map[5][5]);

    move_object(&char_player.object, 80, 64);

    fade_in();
}