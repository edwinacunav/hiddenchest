# * KWinOpen XP
#   Scripter : Kyonides-Arkanthes
#   2019-07-29 - v 0.3.0

# This scriptlet shows you how you can implement window openness a la VX Ace in
# your beloved XP games by using the HiddenChest engine! But it sports not one
# but 4 modes! Yeah one would be the default XP window behavior alias
# "nothing happens". XD

# This shows you how to use it for opening and closing a single menu window.
# If you need to update other stuff as well, check the Scene_Example scriptlet.

# Window_Selectable#update method had to be overridden so place this scriptlet
# right below the Scene_Debug script to make sure it will not be ignored.

# * Script Calls *

# self.open_mode Options: nil false :top :center :bottom
# self.openness = number # Not really necessary if you already set a mode
# open? and close? # In case you need to know if its open or closed
# Window#closing for updating a single menu window that is about to get closed.
# The Window# part stands for any custom window name like @command_window. with
# the dot replacing the # symbol.

class Window_Selectable
  def initialize(x, y, width, height)
    super(x, y, width, height)
    @item_max ||= 1
    @column_max ||= 1
    @index = -1
    @update_height ||= 14
    @open ||= false
    @close ||= false
  end

  def update
    super
    if @open
      self.openness += @update_height
      @open = !open?
      return
    elsif @close
      self.openness -= @update_height
      @close = !close?
      return
    end
    if self.active and @item_max > 0 and @index >= 0
      if Input.repeat?(Input::DOWN)
        if (@column_max == 1 and Input.trigger?(Input::DOWN)) or
           @index < @item_max - @column_max
          $game_system.se_play($data_system.cursor_se)
          @index = (@index + @column_max) % @item_max
        end
      end
      if Input.repeat?(Input::UP)
        if (@column_max == 1 and Input.trigger?(Input::UP)) or
           @index >= @column_max
          $game_system.se_play($data_system.cursor_se)
          @index = (@index - @column_max + @item_max) % @item_max
        end
      end
      if Input.repeat?(Input::RIGHT)
        if @column_max >= 2 and @index < @item_max - 1
          $game_system.se_play($data_system.cursor_se)
          @index += 1
        end
      end
      if Input.repeat?(Input::LEFT)
        if @column_max >= 2 and @index > 0
          $game_system.se_play($data_system.cursor_se)
          @index -= 1
        end
      end
      if Input.repeat?(Input::R)
        if self.top_row + (self.page_row_max - 1) < (self.row_max - 1)
          $game_system.se_play($data_system.cursor_se)
          @index = [@index + self.page_item_max, @item_max - 1].min
          self.top_row += self.page_row_max
        end
      end
      if Input.repeat?(Input::L)
        if self.top_row > 0
          $game_system.se_play($data_system.cursor_se)
          @index = [@index - self.page_item_max, 0].max
          self.top_row -= self.page_row_max
        end
      end
    end
    update_help if self.active and @help_window != nil
    update_cursor_rect
  end

  def closing
    until close?
      Graphics.update
      update
    end if @open_mode # or self.open_mode
    self.visible = false
  end
  def open() @open = true end
  def close() @close = true end
end

class TitleOptionsWindow < Window_Selectable
  def initialize(x, y, w, commands)
    @commands = commands
    @item_max = commands.size
    self.open_mode = :bottom # First Assignment - Ignored by the next one!
    super(x, y, w, @item_max * 32 + 32)
    self.open_mode = :center # End Result: :center
    self.contents = Bitmap.new(width - 32, @item_max * 32)
    refresh
    open
    self.index = 0
  end

  def refresh
    self.contents.clear
    @item_max.times{|i| draw_item(i, normal_color) }
  end

  def draw_item(index, color)
    w = self.contents.width - 8
    # Set a clickable area for each command option - Single Column Style!
    # Delete || in case the areas might change eventually.
    @area[index] ||= [self.x + 16, self.y + 16 + 32 * index, w, 32]
    self.contents.font.color = color
    rect = Rect.new(4, 32 * index, w, 32)
    self.contents.fill_rect(rect, Color.new(0, 0, 0, 0))
    self.contents.draw_text(rect, @commands[index])
  end
  def disable_item(index) draw_item(index, disabled_color) end
end