#include "bus.h"

bool bus_is_complete(std::vector<uint8_t>& inp) {
    size_t i = 0;
    while(i < inp.size()) {
        if(i <= inp.size() - 4 && inp[i] == 'Y' && inp[i + 1] == 'D' && inp[i + 2] == 'T' && inp[i + 3] == '\x91')
            break;
        i++;
    }
    if(i == inp.size()) return false;
    i += 4;
    while(true) {
        if(i >= inp.size()) return false;
        if(inp[i] == TYPE_END) return true;
        if(inp[i] == TYPE_U8) i++;
        else if(inp[i] == TYPE_U16) i += 2;
        else if(inp[i] == TYPE_U32) i += 4;
        else if(inp[i] == TYPE_STR || inp[i] == TYPE_NAME) {
            i++;
            while(i < inp.size() && inp[i] != 0) i++;
        }
        i++;
    }
}

BusData bus_parse(std::vector<uint8_t>& inp) {
    BusData ret;
    String name = "";
    size_t i = 0;
    while(i < inp.size()) {
        if(i <= inp.size() - 4 && inp[i] == 'Y' && inp[i + 1] == 'D' && inp[i + 2] == 'T' && inp[i + 3] == '\x91')
            break;
        i++;
    }
    if(i == inp.size()) return ret;
    i += 4;
    while(true) {
        if(i >= inp.size()) return ret;
        if(inp[i] == TYPE_END) return ret;
        if(inp[i] == TYPE_U8) {
            ret.push_back((BusEntry) { .name = name, .val = { .type = TYPE_U8, .u = inp[++i] } });
        } else if(inp[i] == TYPE_U16) {
            uint16_t n = inp[++i];
            n <<= 8; n |= inp[++i];
            ret.push_back((BusEntry) { .name = name, .val = { .type = TYPE_U16, .u = n } });
        }
        else if(inp[i] == TYPE_U32) {
            uint32_t n = inp[++i];
            n <<= 8; n |= inp[++i];
            n <<= 8; n |= inp[++i];
            n <<= 8; n |= inp[++i];
            ret.push_back((BusEntry) { .name = name, .val = { .type = TYPE_U32, .u = n } });
        }
        else if(inp[i] == TYPE_STR || inp[i] == TYPE_NAME) {
            uint8_t t = inp[i];
            String val = "";
            i++;
            while(i < inp.size() && inp[i] != 0)
                val += (char)inp[i++];
            if(i >= inp.size()) return ret;
            if(t == TYPE_NAME)
                name = val;
            else ret.push_back((BusEntry) { .name = name, .val = { .type = TYPE_STR, .str = val } });
        }
        else if(inp[i] == TYPE_RESET)
            ret.push_back((BusEntry) { .name = name, .val = { .type = inp[i] } });
        i++;
    }
    return ret;
}

std::vector<uint8_t> bus_compile(BusData& data) {
    std::vector<uint8_t> ret;
    ret.push_back('Y'); ret.push_back('D'); ret.push_back('T'); ret.push_back('\x91');
    String last_name = "";
    for(BusEntry& ent : data) {
        if(ent.name != last_name) {
            ret.push_back(TYPE_NAME);
            for(char c : ent.name) ret.push_back(c);
            ret.push_back(0);
            last_name = ent.name;
        }
        ret.push_back(ent.val.type);
        if(ent.val.type == TYPE_U8)
            ret.push_back(ent.val.u);
        else if(ent.val.type == TYPE_U16) {
            ret.push_back(ent.val.u >> 8);
            ret.push_back(ent.val.u);
        }
        else if(ent.val.type == TYPE_U32) {
            ret.push_back(ent.val.u >> 24);
            ret.push_back(ent.val.u >> 16);
            ret.push_back(ent.val.u >> 8);
            ret.push_back(ent.val.u);
        }
        else if(ent.val.type == TYPE_STR) {
            for(char c : ent.val.str) ret.push_back(c);
            ret.push_back(0);
        }
        else if(ent.val.type == TYPE_RESET) {
            // pass
        }
    }
    ret.push_back(TYPE_END);
    return ret;
}