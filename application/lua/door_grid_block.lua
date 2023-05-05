require("application.lua.utils")
require("math")

DoorGridBlock = {
    percent_in_cell = 0.05,

    mat = {},
}

local function count_max_continuous(vector)
    local i, j = 0, 0
    for k = 1, table_size(vector) do
        if vector[k] then
            i = i + 1
        else
            j = math.max(i, j)
            i = 0
        end
    end
    return math.max(i, j)
end

function DoorGridBlock.process()
    local bool_mat = {}
    local raw_mat = DoorGridBlock.mat
    local row_count = #raw_mat
    local col_count = #raw_mat[1]
    for i = 1, row_count do
        local row = raw_mat[i]
        local bool_vector = {}
        for j = 1, col_count do
            if row[j] > DoorGridBlock.percent_in_cell then
                table.insert(bool_vector, true)
            else
                table.insert(bool_vector, false)
            end
        end
        table.insert(bool_mat, bool_vector)
    end

    print_bool_mat(bool_mat, 6, 8)

    local index_height = {}
    for i = 1, col_count do
        local col_vector = {}
        for j = 1, row_count do
            table.insert(col_vector, bool_mat[j][i])
        end
        local index, height = i, count_max_continuous(col_vector)
        if height > (row_count + 1) / 2 then
            table.insert(index_height, { index = index, height = height })
        end
    end

    local dev = 0.0
    local deg = 0.0
    local confidence = 1.0
    if #index_height == 2 then
        --  +---+---+---+---+---+---+
        --  | # |   |   |   | # |   |
        --  +-#-+---+---+---+-#-+---+
        --  | # |   |   |   | # |   |
        --  +-#-+---+---+---+-#-+---+
        --  | # |   |   |   | # |   |
        --  +-#-+---+---+---+-#-+---+
        --  | ################# |   |
        --  +---+---+---+---+---+---+
        --  |   |   |   |   |   |   |
        --  +---+---+---+---+---+---+
        local col_left = index_height[1]
        local col_right = index_height[2]

        local door_width = col_right.index - col_left.index
        if door_width > 1 then
            local center = (1 + col_count) / 2
            local center_lr = (col_left.index + col_right.index) / 2
            dev = (center_lr - center) / (col_count / 4)

            local min_height = math.min(col_left.height, col_right.height)
            if door_width < min_height and col_left.height ~= col_right.height then
                deg = 1.0 - door_width / min_height
                deg = deg * 45.0
                if col_right.height - col_left.height < 0 then
                    deg = deg * -1
                end
            end
        else
            if col_left.height < col_right.height then
                table.remove(index_height, 1)
            else
                table.remove(index_height, 2)
            end
        end
    end

    if #index_height == 1 then
        local index, height = index_height[1].index, index_height[1].height
        local function get_around_bool()
            local l, r, bl, br, tl, tr
            if index - 1 < 1 then
                l, bl, tl = false, false, false
            end
            if index + 1 > col_count then
                r, br, tr = false, false, false
            end
            if height - 1 < 1 then
                tl, tr = false, false
            end
            if height + 1 > row_count then
                bl, br = false, false
            end

            if type(l) == "nil" then
                l = bool_mat[height][index - 1]
            end
            if type(r) == "nil" then
                r = bool_mat[height][index + 1]
            end
            if type(bl) == "nil" then
                bl = bool_mat[height + 1][index - 1]
            end
            if type(br) == "nil" then
                br = bool_mat[height + 1][index + 1]
            end
            if type(tl) == "nil" then
                tl = bool_mat[height - 1][index - 1]
            end
            if type(tr) == "nil" then
                tr = bool_mat[height - 1][index + 1]
            end
        end
        local l, r, bl, br, tl, tr = get_around_bool()

        if r or br or tr then
            -- +---+---+---+
            -- |   |   |   |
            -- +---+---+---+
            -- |   | # |   |
            -- +---+-#-+---+
            -- |   | ######|
            -- +---+---+---+
            print("左下角")
            confidence = confidence / 2
            dev = index / col_count
        end
        if l or bl or tl then
            -- +---+---+---+
            -- |   |   |   |
            -- +---+---+---+
            -- |   | # |   |
            -- +---+-#-+---+
            -- | ##### |   |
            -- +---+---+---+
            print("右下角")
            confidence = confidence / 2
            dev = -(col_count - index) / col_count
        end

        if tl or br then
            -- +---+-#-+---+
            -- |   | # |   |
            -- +---+-#-+---+
            -- |   | ##|   |
            -- +---+---##--+
            -- |   |   | # |
            -- +---+---+---+
            print("向右旋转")
            deg = 0.5
            goto deg_finish
        end
        if tr or bl then
            -- +-#-+---+---+
            -- | # |   |   |
            -- +-#-+--#+---+
            -- | # | # |   |
            -- +-#-##--+---+
            -- | ##|   |   |
            -- +---+---+---+
            print("向左旋转")
            deg = -0.5
            goto deg_finish
        end

        -- +---+-#-+---+
        -- |   | # |   |
        -- +---+-#-+---+
        -- |   | # |   |
        -- +---+-#-+---+
        -- |   | # |   |
        -- +---+---+---+
        confidence = confidence / 2
        local mid_index = (col_count + 1) / 2
        if index < mid_index then
            print("右平移")
            dev = index / mid_index
        else
            print("左平移")
            dev = -(col_count - index) / mid_index
        end
    end
    ::deg_finish::
    print(deg, dev)
end

function DoorGridBlock.update(raw_vision_results)
    DoorGridBlock.mat = raw_vision_results
end
