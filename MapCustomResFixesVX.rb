# Increased HiddenChest VX Resolution Map Fix
# * Scripter : Kyonides-Arkanthes
# 2019-01-21

class Game_Player
  CENTER_X = (Graphics.width / 2 - 16) * 8
  CENTER_Y = (Graphics.height / 2 - 16) * 8
end

class Game_Map
  def scroll_down(distance)
    if loop_vertical?
      @display_y += distance
      @display_y %= @map.height * 256
      @parallax_y += distance
    else
      last_y = @display_y
      tiles_max = Graphics.height / 32
      @display_y = [last_y + distance, (height - tiles_max) * 128].min
      @parallax_y += @display_y - last_y
    end
  end

  def scroll_right(distance)
    if loop_horizontal?
      @display_x += distance
      @display_x %= @map.width * 256
      @parallax_x += distance
    else
      last_x = @display_x
      tiles_max = Graphics.width / 32
      @display_x = [last_x + distance, (width - tiles_max) * 128].min
      @parallax_x += @display_x - last_x
    end
  end
end

class Spriteset_Map
  def create_viewports
    w = Graphics.width
    h = Graphics.height
    @viewport1 = Viewport.new(0, 0, w, h)
    @viewport2 = Viewport.new(0, 0, w, h)
    @viewport3 = Viewport.new(0, 0, w, h)
    @viewport2.z = 50
    @viewport3.z = 100
  end
end