# * Scene_Example Script
#   Scripter : Kyonides-Arkanthes

#   This script consists of ideas on How To Implement HiddenChest's
#   New Script calls like...
#   Window#open_mode Window#open and Window#close or
#   Sprite#reduce_speed or Sprite#increase_width! or Sprite#reduce_width!

#   It will update a menu window and two sets of sprites, faces and some
#   custom pictures. It will not sport any sort of backdrop.

#   Scene Script Call: $scene = Scene_Example.new

module Examples
  PICTURE = "Some Picture"
  FACES = ["", "", "", ""]
end

class Scene_Example
  include Examples
  def main
    create_interphase
    Graphics.transition
    entry_animations
    loop do
      Graphics.update
      Input.update
      update
      break if $scene != self
    end
    exit_animations
    Graphics.freeze
    terminate # Disposal of all windows and sprites
  end

  def create_interphase
    # You can create all windows or sprites here like...
    @sprites = []
    3.times do |n|
      @sprites << sprite = Sprite.new
      sprite.x = 8 + n * 164
      sprite.y = 40
      sprite.bitmap = RPG::Cache.picture(PICTURE).dup
      sprite.reduce_speed = 12 # How many pixels will be updated
      sprite.increase_width!
    end
    @last_sprite = @sprites[-1]
    @faces = []
    4.times do |n|
      @faces << sprite = Sprite.new
      sprite.x = 16 + n * 104
      sprite.y = 160
      sprite.bitmap = RPG::Cache.face(FACES[n]).dup
      sprite.increase_width!
    end
    @last_face = @faces[-1]
    @command_window = Window_Command.new(160, ["Hi!", "Bye!"])
    @command_window.x = (Graphics.width - @command_window.width) / 2
    @command_window.y = 340
    @command_window.open_mode = :bottom
    @command_window.open
  end

  def entry_animations
    # You keep them updated till they reach the maximum width
    until @last_sprite.increased_width? and @last_face.increased_width?
      Graphics.update
      @sprites.each{|face| face.update }
      @faces.each{|face| face.update }
      @command_window.update
    end # No extra Graphics update cycle needed here
  end

  def exit_animations
    # You keep them updated till they reach the minimum width
    until @last_sprite.reduced_width? and @last_face.reduced_width?
      Graphics.update
      @sprites.each{|face| face.update }
      @faces.each{|face| face.update }
      @command_window.update
    end
    @command_window.visible = false
    Graphics.update # Extra cycle as some sort of visual failsafe
  end

  def terminate
    @command_window.dispose
    (@faces + @sprites).each do |sprite|
      sprite.bitmap.dispose
      sprite.dispose
    end
  end

  def update
    @command_window.update
    if Input.trigger?(Input::B)
      Audio.play_cancel
      return $scene = nil
    elsif Input.trigger?(Input::C)
      Audio.play_ok
      $scene = case @command_window.index
      when 0 then Scene_Title.new
      when 1 then nil
      end
      @command_window.close
      (@faces + @sprites).each{|s| s.reduce_width! }
    end
  end
end