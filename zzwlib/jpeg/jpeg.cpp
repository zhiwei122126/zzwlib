
/*
 * 解码过程： jpeg 标准中， image,frame,scan 之间的关系为：
 * 1. image 中包含1 个 或 多个 frame；
 *  对于 顺序（sequential）模式和 progressive 模式， 一个 image 中只有 1 个 frame；
 *  对于 hierarchical 模式， 一个 image 中包含多个 frame；
 * 2. frame 中包含 1 个 或 多个 scan；
 * 3. jpeg 编码的最小单元 MCU （Minimum Coding Unit）， 一个scan 包含 1 个或多个 MCU；
 * 4. 一个 MCU 由 1个或多个数据单元（data unit)构成，数据单元是 8x8 的数据块。
 *  对于非交错存放方式， MCU 由 1 个数据单元构成；对于交错存放方式，MCU 由 多个数据单元构成
 *  每通道 一个 或多个；
 * 举例：
 * 图 1 中，第一个MCU 包括 A1 B1 C1 三个数据单元；
 * 图 2 中，第一个MCU 包含 A1 A2 B1 C1 4个数据单元
*/

#include <stdint.h>
#include <memory>
#include <vector>
#include <array>
#include <cmath>

#include "jpeg.hpp"
#include "../logger.hpp"

zzwlib::logger  jpeg_logger("jpeg", zzwlib::loglevel::log_verbose_level);

namespace zzwlib{
    namespace jpeg {

class jpeg_marker {
public:
    // must based on uint, convert MARKER start with 0xFx correctly.
    enum class type : uint32_t {
        M_SOF0  = 0xC0,
        M_SOF1  = 0xC1,
        M_SOF2  = 0xC2,
        M_SOF3  = 0xC3,
        M_DHT   = 0xC4,
        M_SOF5  = 0xC5,
        M_SOF6  = 0xC6,
        M_SOF7  = 0xC7,
        M_JPG   = 0xC8,
        M_SOF9  = 0xC9,
        M_SOF10 = 0xCA,
        M_SOF11 = 0xCB,
        M_DAC   = 0xCC,
        M_SOF13 = 0xCD,
        M_SOF14 = 0xCE,
        M_SOF15 = 0xCF,
        M_RST0  = 0xD0,
        M_RST1  = 0xD1,
        M_RST2  = 0xD2,
        M_RST3  = 0xD3,
        M_RST4  = 0xD4,
        M_RST5  = 0xD5,
        M_RST6  = 0xD6,
        M_RST7  = 0xD7,
        M_SOI   = 0xD8,
        M_EOI   = 0xD9,
        M_SOS   = 0xDA,
        M_DQT   = 0xDB,
        M_DNL   = 0xDC,
        M_DRI   = 0xDD,
        M_DHP   = 0xDE,
        M_EXP   = 0xDF,
        M_APP0  = 0xE0,
        M_APP1  = 0xE1,
        M_APP2  = 0xE2,
        M_APP3  = 0xE3,
        M_APP4  = 0xE4,
        M_APP5  = 0xE5,
        M_APP6  = 0xE6,
        M_APP7  = 0xE7,
        M_APP8  = 0xE8,
        M_APP9  = 0xE9,
        M_APP15 = 0xEF,
        M_JPG0  = 0xF0,
        M_JPG13 = 0xFD,
        M_COM   = 0xFE,

        M_TEM   = 0x01,
        M_ERROR = 0x100,
        RST0   = 0xD0
    };
    static const char *to_string(type t) {
        switch (t) {
        case type::M_SOF0: return "SOF0";
        case type::M_SOF1: return "SOF1";
        case type::M_SOF2: return "SOF2";
        case type::M_SOF3: return "SOF3";
        case type::M_DHT: return "DHT";
        case type::M_SOF5: return "SOF5";
        case type::M_SOF6: return "SOF6";
        case type::M_SOF7: return "SOF7";
        case type::M_JPG: return "JPG";
        case type::M_SOF9: return "SOF9";
        case type::M_SOF10: return "SOF10";
        case type::M_SOF11: return "SOF11";
        case type::M_DAC: return "DAC";
        case type::M_SOF13: return "SOF13";
        case type::M_SOF14: return "SOF14";
        case type::M_SOF15: return "SOF15";
        case type::M_RST0: return "RST0";
        case type::M_RST1: return "RST1";
        case type::M_RST2: return "RST2";
        case type::M_RST3: return "RST3";
        case type::M_RST4: return "RST4";
        case type::M_RST5: return "RST5";
        case type::M_RST6: return "RST6";
        case type::M_RST7: return "RST7";
        case type::M_SOI: return "SOI";
        case type::M_EOI: return "EOI";
        case type::M_SOS: return "SOS";
        case type::M_DQT: return "DQT";
        case type::M_DNL: return "DNL";
        case type::M_DRI: return "DRI";
        case type::M_DHP: return "DHP";
        case type::M_EXP: return "EXP";
        case type::M_APP0: return "APP0";
        case type::M_APP1: return "APP1";
        case type::M_APP2: return "APP2";
        case type::M_APP3: return "APP3";
        case type::M_APP4: return "APP4";
        case type::M_APP5: return "APP5";
        case type::M_APP6: return "APP6";
        case type::M_APP7: return "APP7";
        case type::M_APP8: return "APP8";
        case type::M_APP9: return "APP9";
        case type::M_APP15: return "APP15";
        case type::M_JPG0: return "JPG0";
        case type::M_JPG13: return "JPG13";
        case type::M_COM: return "COM";
        case type::M_TEM: return "TEM";
        case type::M_ERROR: return "ERROR";
        default: return "UNKNOWN";
        }
    }
};

const std::pair<int, int> zzOrderToMatIndices(int zzIndex)
{
    static const std::pair<int, int> zzOrderToMatIndices[64] = {
        { 0, 0 },
        { 0, 1 }, { 1, 0 },
        { 2, 0 }, { 1, 1 }, { 0, 2 },
        { 0, 3 }, { 1, 2 }, { 2, 1 }, { 3, 0 },
        { 4, 0 }, { 3, 1 }, { 2, 2 }, { 1, 3 }, { 0, 4 },
        { 0, 5 }, { 1, 4 }, { 2, 3 }, { 3, 2 }, { 4, 1 }, { 5, 0 },
        { 6, 0 }, { 5, 1 }, { 4, 2 }, { 3, 3 }, { 2, 4 }, { 1, 5 }, { 0, 6 },
        { 0, 7 }, { 1, 6 }, { 2, 5 }, { 3, 4 }, { 4, 3 }, { 5, 2 }, { 6, 1 }, { 7, 0 },
        { 7, 1 }, { 6, 2 }, { 5, 3 }, { 4, 4 }, { 3, 5 }, { 2, 6 }, { 1, 7 },
        { 2, 7 }, { 3, 6 }, { 4, 5 }, { 5, 4 }, { 6, 3 }, { 7, 2 },
        { 7, 3 }, { 6, 4 }, { 5, 5 }, { 4, 6 }, { 3, 7 },
        { 4, 7 }, { 5, 6 }, { 6, 5 }, { 7, 4 },
        { 7, 5 }, { 6, 6 }, { 5, 7 },
        { 6, 7 }, { 7, 6 },
        { 7, 7 }
    };
    return zzOrderToMatIndices[zzIndex];
}

int valCategory(int val)
{
    if (val < 0) {
        val = -val;
    }
    if (val == 0) {
        return 0;
    }
    int n = 0;
    while (val > 0) {
        n++;
        val >>= 1;
    }
    return n;
}

int matIndicesToZOrder(int i, int j)
{
    static const int matZOrder[8][8] = {
        { 0, 1, 5, 6, 14, 15, 27, 28 },
        { 2, 4, 7, 13, 16, 26, 29, 42 },
        { 3, 8, 12, 17, 25, 30, 41, 43 },
        { 9, 11, 18, 24, 31, 40, 44, 53 },
        { 10, 19, 23, 32, 39, 45, 52, 54 },
        { 20, 22, 33, 38, 46, 51, 55, 60 },
        { 21, 34, 37, 47, 50, 56, 59, 61 },
        { 35, 36, 48, 49, 57, 58, 62, 63 }
    };
    return matZOrder[i][j];
}

struct Node {
    bool root;
    bool leaf;
    std::string code;
    int  value;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    std::shared_ptr<Node> parent;
    Node() :
        root(false),
        leaf(false),
        code(""),
        value(0),
        left(nullptr),
        right(nullptr),
        parent(nullptr)
    {}
    Node(const std::string &code_, int value_) :
        root(false),
        leaf(false),
        code(code_),
        value(value_),
        left(nullptr),
        right(nullptr),
        parent(nullptr)
    {}
};

// Alias for a node
typedef std::shared_ptr<Node> NodePtr;

NodePtr create_root_node(int value)
{
    NodePtr root = std::make_shared<Node>("",value);
    root->root = true;
    return root;
}

void insert_left(NodePtr node, int value)
{
    if (node == nullptr) {
        LOGE(jpeg_logger, "insert_left, node is null");
        return;
    }
    if (node->left != nullptr) {
        LOGE(jpeg_logger, "insert_left, node already has left child");
        return;
    }
    NodePtr left_node = std::make_shared<Node>(node->code + "0", value);
    node->left = left_node;
    left_node->parent = node;
}


void insert_right(NodePtr node, int value)
{
    if (node == nullptr) {
        LOGE(jpeg_logger, "insert_right, node is null");
        return;
    }
    if (node->right!= nullptr) {
        LOGE(jpeg_logger, "insert_right, node already has right child");
        return;
    }
    NodePtr right_node = std::make_shared<Node>(node->code + "1", value);
    node->right = right_node;
    right_node->parent = node;
}

NodePtr same_level_right(NodePtr node)
{
    if (node == nullptr) {
        LOGE(jpeg_logger, "same_level_right, node is null");
        return nullptr;
    }
    if (node->parent == nullptr) {
        LOGE(jpeg_logger, "same_level_right, node has no parent");
        return nullptr;
    }
    // node is the left child of its parent, so its right sibling is its parent's right child
    if (node ==  node->parent->left) {
        return node->parent->right;
    }

    // node is the right child of its parent, traverse back the tree to find the first right sibling
    int count = 0;
    NodePtr p = node;
    while(p->parent != nullptr && p->parent->right == p) {
        count++;
        p = p->parent;
    }
    // while loop ends when p->parent == nullptr
    if (p->parent == nullptr) {
        LOGE(jpeg_logger, "same_level_right, node has no right sibling");
        return nullptr;
    }
    // while loop ends when p->parent->right != p; p is its parent's left child.
    // get its parent's right child
    p = p->parent->right;
    // traverse down the tree to find the first left child
    while (count > 0) {
        p = p->left;
        count--;
    }

    return p;
}

void in_order_traverse(NodePtr node)
{
    if (node == nullptr) {
        return;
    }
    in_order_traverse(node->left);
    if (node->leaf) {
        LOGD(jpeg_logger, "node->code: %s, node->value: %d", node->code.c_str(), node->value);
    }
    in_order_traverse(node->right);
}

struct HuffmanRow {
    int num;
    std::vector<uint8_t> val_list;
};

typedef std::vector<HuffmanRow> HuffmanTable;

class HuffmanTree
{
    public:
        HuffmanTree(): m_root(nullptr) {}

        HuffmanTree( const HuffmanTable& htable )
        {
            constructHuffmanTree( htable );
        }
        void constructHuffmanTree(const HuffmanTable &table)
        {
            m_root = create_root_node(0);
            insert_left(m_root, 0);
            insert_right(m_root, 0);

            NodePtr left_most = m_root->left;
            // if the count is 0, add left & right children for all nodes;
            for (auto i = 1; i <= 16; ++i) {
                if (table[i-1].num == 0) {
                    for (NodePtr p_nd = left_most; p_nd != nullptr; p_nd = same_level_right(p_nd)) {
                        insert_left(p_nd, 0);
                        insert_right(p_nd, 0);
                    }
                    left_most = left_most->left;
                } else {
                    // assign codes starting from left_most in the tree
                    for (auto && value : table[i-1].val_list) {
                        left_most->value = value;
                        left_most->leaf = true;
                        left_most = same_level_right(left_most);
                    }

                    insert_left(left_most, 0);
                    insert_right(left_most, 0);
                    NodePtr p_nd = same_level_right(left_most);
                    left_most = left_most->left;

                    while (p_nd!= nullptr) {
                        insert_left(p_nd, 0);
                        insert_right(p_nd, 0);
                        p_nd = same_level_right(p_nd);
                    }
                }
            }
        }
        const NodePtr getTree() const { return m_root;}

    private:
        NodePtr m_root;
};

struct MCU {
    MCU() = default;

    MCU(const std::vector<int>& compRLE,
        const std::vector<uint16_t>& QTable,
        int &dc_sum) {
            constructMCU(compRLE, QTable, dc_sum);
    }

    void constructMCU(const std::vector<int>& compRLE,
                      const std::vector<uint16_t>& QTable, int &dc_sum) {

        LOGD(jpeg_logger, "constructMCU, index: %d", m_index);
    
        // initialize the zzorder with all zeros
        std::array<int, 64> zzorder;
        std::fill(zzorder.begin(), zzorder.end(), 0);
        int j = -1;

        // construct the zzorder array
        for (int i = 0; i <= compRLE.size() - 2; i += 2) {
            if (compRLE[i] == 0 && compRLE[i + 1] == 0) {
                break;
            }
            // skip the number of zeros
            j = j + compRLE[i] + 1;
            zzorder[j] = compRLE[i + 1];
        }

        // DC_i = DC_i-1 + diff
        zzorder[0] += dc_sum;
        dc_sum = zzorder[0];

        for (int i = 0; i < 64; i++) {
            zzorder[i] *= QTable[i];
        }

        // compute the zigzag order
        for (int i = 0; i < 64; i++) {
            auto [row, col] = zzOrderToMatIndices(i);
            m_dct_dst[row][col] = zzorder[i];
        }
        computeIDCT();
    }

    void computeIDCT() {
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                float sum = 0.0f;
                for (int u = 0; u < 8; u++) {
                    for (int v = 0; v < 8; v++) {
                        float alpha = (u == 0) ? 1.0f / std::sqrt(2.0f) : 1.0f;
                        float beta =  (v == 0) ? 1.0f / std::sqrt(2.0f) : 1.0f;
                        sum += alpha * beta * m_dct_dst[u][v] * std::cos((2 * x + 1) * u * M_PI / 16.0) * std::cos((2 * y + 1) * v * M_PI / 16.0);
                    }
                }
                m_dct_src[y][x] = std::roundf( sum / 4.0f) + 128;
            }
        }
    }

    std::array<std::array<uint8_t, 8>, 8> m_dct_src;
    std::array<std::array<float, 8>, 8>  m_dct_dst;
    int m_index;
};


int m_image_x_size;
int m_image_y_size;
int m_comps_in_frame;            // # of components in frame
int m_comp_h_samp[4];       // component's horizontal sampling factor
int m_comp_v_samp[4];       // component's vertical sampling factor
int m_comp_quant[4];        // component's quantization table selector
int m_comp_ident[4];        // component's ID

std::vector<std::vector<uint16_t>> m_quant_tbl;
HuffmanTable m_huffmanTable[2][2];
HuffmanTree m_huffmanTree[2][2];

int m_comps_in_scan;
std::vector<uint8_t> m_scan_bytes;

// return: 0 or positive, next marker pos.
uint32_t next_marker(uint8_t *data, int len, int &pos) {
    pos = 0;
    while (pos < len - 1) {
        if (data[pos] == 0xff) {
            uint32_t marker = data[pos + 1];
            if (marker == 0xff) {
                pos += 1;
                continue;
            }
            if (marker == 0x00) {
                pos += 2;
                continue;
            }
            return marker;
        }
        pos++;
    }
    return 0;
}

int dqt_marker(uint8_t *data, int len, int &marker_len)
{
    // 2 bytes marker
    // 2 bytes - len
    // 1 byte - low 4 bits: table id; high 4 bits: precision (0: 8-bit, 1: 16-bit)
    // 64 * (precision + 1) bytes - table data

    int cur_pos = 0;
    int left_bytes = len;
    // skip marker & length
    cur_pos += 2;
    left_bytes -= 2;

    marker_len = (data[cur_pos] << 8) | data[cur_pos + 1];
    cur_pos += 2;

    if (left_bytes < marker_len) {
        LOGD(jpeg_logger, "left bytes %d < marker len %d", left_bytes, marker_len);
        return -1;
    }
    left_bytes = marker_len - 2;

    // 1 byte - low 4 bits: table id; high 4 bits: precision (0: 8-bit, 1: 16-bit)
    auto table_id = data[cur_pos] & 0x0f;
    auto precision = (data[cur_pos] >> 4) & 0x0f;
    cur_pos += 1;

    LOGD(jpeg_logger, "table id %d, precision %d, table:", table_id, precision);

    while (m_quant_tbl.size() <= table_id) {
        m_quant_tbl.push_back(std::vector<uint16_t>());
    }
    for (int i = 0; i < 64; i++) {
        uint16_t val = 0;
        if (precision == 0) {
            val = data[cur_pos];
            cur_pos += 1;
        } else if (precision == 1) {
            val = (data[cur_pos] << 8) | data[cur_pos + 1];
            cur_pos += 2;
        } else {
            LOGD(jpeg_logger, "invalid precision %d", precision);
        }
        LOGD(jpeg_logger, " 0x%x ", val);
        m_quant_tbl[table_id].push_back(val);
    }

    return 0;
}

int dht_marker(uint8_t *data, int len, int &marker_len)
{
    // 2 bytes marker
    // 2 bytes - len
    // 1 byte - low 4 bits: table id; high 4 bits: table type (0 : DC, 1: AC)
    // 16 bytes - table data

    int cur_pos = 0;
    int left_bytes = len;
    // skip marker & length
    cur_pos += 2;
    left_bytes -= 2;

    marker_len = (data[cur_pos] << 8) | data[cur_pos + 1];
    cur_pos += 2;

    if (left_bytes < marker_len) {
        LOGD(jpeg_logger, "left bytes %d < marker len %d", left_bytes, marker_len);
        return -1;
    }
    left_bytes = marker_len - 2;

    // 1 byte - low 4 bits: table id; high 4 bits: table type (0 : DC, 1: AC)
    auto table_id = data[cur_pos] & 0x0f;
    auto table_type = (data[cur_pos] >> 4) & 0x0f;
    cur_pos += 1;
    LOGD(jpeg_logger, "table id %d, table type %d(%s), table:", table_id, table_type, table_type == 0 ? "DC" : "AC");

    int count = 0;

    for (int i = 1; i <= 16; i++) {
        HuffmanRow row;
        row.num = data[cur_pos];
        cur_pos += 1;
        m_huffmanTable[table_type][table_id].push_back(row);

        count += row.num;
        LOGD(jpeg_logger, " 0x%x ", row.num);
    }
    int row_idx = 0;
    for (int i = 0; i < count; i++) {
        uint8_t code = data[cur_pos];
        cur_pos += 1;

        while(m_huffmanTable[table_type][table_id][row_idx].num == 0) {
            row_idx++;
        }
        m_huffmanTable[table_type][table_id][row_idx].val_list.push_back(code);
        if (m_huffmanTable[table_type][table_id][row_idx].num == m_huffmanTable[table_type][table_id][row_idx].val_list.size())
            row_idx++;

        LOGD(jpeg_logger, " 0x%x ", code);
    }

    m_huffmanTree[table_type][table_id].constructHuffmanTree(m_huffmanTable[table_type][table_id]);

    return 0;
};

int sof0_marker(uint8_t *data, int len, int &marker_len)
{
    // 2 bytes marker
    // 2 bytes - len
    // 1 byte - sample_precision
    // 2 bytes - height
    // 2 bytes - width
    // 1 byte - num_components
    // num_components bytes * 3 - component info

    int cur_pos = 0;
    int left_bytes = len;

    // skip marker & length
    cur_pos += 2;
    left_bytes -= 2;

    marker_len = (data[cur_pos] << 8) | data[cur_pos + 1];
    cur_pos += 2;

    if (left_bytes < marker_len) {
        LOGD(jpeg_logger, "left bytes %d < marker len %d", left_bytes, marker_len);
        return -1;
    }
    left_bytes = marker_len - 2;

    auto precision = data[cur_pos];
    cur_pos += 1;
    if (precision != 8) {
        LOGD(jpeg_logger, "sample precision %d, only support precision 8", precision);
    }

    m_image_y_size = (data[cur_pos] << 8) | data[cur_pos + 1];
    cur_pos += 2;

    m_image_x_size = (data[cur_pos] << 8) | data[cur_pos + 1];
    cur_pos += 2;

    m_comps_in_frame = data[cur_pos];
    cur_pos += 1;

    for (int i = 0; i < m_comps_in_frame; i++)
    {
        m_comp_ident[i]  = data[cur_pos]; cur_pos += 1;
        m_comp_h_samp[i] = data[cur_pos] & 0x0f;
        m_comp_v_samp[i] = (data[cur_pos] >> 4) & 0x0f;
        cur_pos += 1;
        m_comp_quant[i]  = data[cur_pos]; cur_pos += 1;
        LOGD(jpeg_logger, "component %d, h_samp %d, v_samp %d, quant %d",
            m_comp_ident[i], m_comp_h_samp[i], m_comp_v_samp[i], m_comp_quant[i]);
    }
    LOGD(jpeg_logger, "image size %dx%d, %d components", m_image_x_size, m_image_y_size, m_comps_in_frame);

    if (marker_len != cur_pos - 2)  {
        LOGD(jpeg_logger, "sof0 marker len %d, actual len %d", marker_len, cur_pos - 2);
    }
    return 0;
}

int sos_marker(uint8_t *data, int len, int &marker_len)
{
    // 2 bytes marker
    // 2 bytes - len
    // 1 byte - num_components
    // num_components bytes * 2 - component info
    // 1 byte spectral start
    // 1 byte spectral end
    // 1 byte low 4 bits: successive_high; high 4 bits: successive_low

    int cur_pos = 0;
    int left_bytes = len;

    // skip marker & length
    cur_pos += 2;
    left_bytes -= 2;

    marker_len = (data[cur_pos] << 8) | data[cur_pos + 1];
    cur_pos += 2;

    if (left_bytes < marker_len) {
        LOGD(jpeg_logger, "left bytes %d < marker len %d", left_bytes, marker_len);
        return -1;
    }
    left_bytes = marker_len - 2;

    m_comps_in_scan = data[cur_pos];
    cur_pos += 1;

    for (int i = 0; i < m_comps_in_scan; i++) {
        // 1byte的颜色分量id，
        // 1byte的直流/交流系数表号（高4位：直流分量所使用的哈夫曼树编号，低4位：交流分量使用的哈夫曼树的编号）
        auto comp_id = data[cur_pos]; cur_pos += 1;

        auto ac_huff_table_id = data[cur_pos] & 0x0f;
        auto dc_huff_table_id = (data[cur_pos] >> 4) & 0x0f;
        cur_pos += 1;

        LOGD(jpeg_logger, "component %d, ac_huff_table_id %d, dc_huff_table_id %d",
            comp_id, ac_huff_table_id, dc_huff_table_id);
    }

    return 0;
}

int scan_image_data(uint8_t *data, int len, int &data_len)
{
    int cur_pos = 0;
    int left_bytes = len;

    while(left_bytes > 0) {
        if (data[cur_pos] == 0xff && data[cur_pos + 1] == 0x00) {
            m_scan_bytes.push_back(0xff);
            left_bytes -= 2;
            cur_pos += 2;
        } else if (data[cur_pos] == 0xff && data[cur_pos + 1] == 0xd9) {
            data_len = cur_pos;
            return 0;
        } else {
            m_scan_bytes.push_back(data[cur_pos]);
            left_bytes -= 1;
            cur_pos += 1;
        }
    }
    return 0;
}

int decode(uint8_t *data, int len)
{
    int cur_pos = 0;
    int left_bytes = len;

    while(left_bytes > 0) {
        int marker_pre_offset = 0;

        auto marker = static_cast<jpeg_marker::type>(
            next_marker(data + cur_pos, left_bytes, marker_pre_offset));

        LOGD(jpeg_logger, "next marker 0x%X (%s), cur 0x%X marker_pre_offset %d, pos 0x%x",
            marker, jpeg_marker::to_string(marker),
            cur_pos, marker_pre_offset,
            cur_pos + marker_pre_offset);

        switch ((marker)) {
            case jpeg_marker::type::M_SOI:
            case jpeg_marker::type::M_EOI:
                cur_pos += (marker_pre_offset + 2);
                left_bytes -= (marker_pre_offset + 2);
                break;
            case jpeg_marker::type::M_DQT: {
                int marker_len = 0;
                dqt_marker(data + cur_pos + marker_pre_offset, left_bytes, marker_len);
                cur_pos += (marker_pre_offset + 2 + marker_len);
                left_bytes -= (marker_pre_offset + 2 + marker_len);
            } break;
            case jpeg_marker::type::M_SOF0: {
                int marker_len = 0;
                sof0_marker(data + cur_pos + marker_pre_offset, left_bytes, marker_len);
                cur_pos += (marker_pre_offset + 2 + marker_len);
                left_bytes -= (marker_pre_offset + 2 + marker_len);
            } break;
            case jpeg_marker::type::M_DHT: {
                int marker_len = 0;
                dht_marker(data + cur_pos + marker_pre_offset, left_bytes, marker_len);
                cur_pos += (marker_pre_offset + 2 + marker_len);
                left_bytes -= (marker_pre_offset + 2 + marker_len);
            } break;
            case jpeg_marker::type::M_SOS: {
                int marker_len = 0;
                sos_marker(data + cur_pos + marker_pre_offset, left_bytes, marker_len);
                cur_pos += (marker_pre_offset + 2 + marker_len);
                left_bytes -= (marker_pre_offset + 2 + marker_len);

                int data_len = 0;
                scan_image_data(data + cur_pos, left_bytes, data_len);
                cur_pos += data_len;
                left_bytes -= data_len;
            } break;
            default:
                cur_pos += (marker_pre_offset + 2);
                left_bytes -= (marker_pre_offset + 2);
                int app_len = (data[cur_pos] << 8) | data[cur_pos + 1];
                LOGD(jpeg_logger, "%s len %d", jpeg_marker::to_string(marker), app_len);
                cur_pos += app_len;
                left_bytes -= app_len;
                break;
        }
    }

    if (m_scan_bytes.size() == 0) {
        LOGD(jpeg_logger, "scan bytes is empty");
        return -1;
    } else {
        LOGD(jpeg_logger, "scan bytes size %d", m_scan_bytes.size());
        // process scan data via huffman tree

    }
    return 0;
}

} // namespace jpeg

} // zzwlib

std::shared_ptr<uint8_t[]> read_file(const char *path, int &len)
{
    std::shared_ptr<uint8_t[]> invalid_data(nullptr);
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        LOGE(jpeg_logger, "open file %s failed", path);
        return invalid_data;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    std::shared_ptr<uint8_t[]> data(new uint8_t[len]);
    auto read_bytes = fread(data.get(), 1, len, fp);
    fclose(fp);
    if (read_bytes != len) {
        LOGE(jpeg_logger, "read file %s failed, read %d bytes, expect %d bytes", path, read_bytes, len);
        return invalid_data;
    }
    return data;
}


int main(int argc, char *argv[])
{
    int file_len = 0;
    auto data = read_file("test.jpg", file_len);
    zzwlib::jpeg::decode(data.get(), file_len);
    return 0;
}
