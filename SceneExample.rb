# * Scene_Example Script
#   Scripter : Kyonides-Arkanthes
#   2019-08-06

#   This script consists of ideas on How To Implement HiddenChest's
#   New Script calls like...
#   Window#open_mode Window#open and Window#close or
#   Sprite#reduce_speed or Sprite#increase_width! or Sprite#reduce_width!

#   It will update a menu window and two sets of sprites, faces and some
#   custom pictures. It will not sport any sort of backdrop.

#   Scene Script Call: $scene = Scene_Example.new

module Examples
  PICTURE = 'equipbox'
  TITLEBAR = 'leather bar'
  CLOSEICON = 'cancel24'
  FACES = Dir['Graphics/Faces/*'].sort[0..3].map{|s| s.sub(".png","") }
  FACES.map!{|s| s.sub("Graphics/Faces/","") }
end

class BaseScene
  def main_loop
    loop do
      Graphics.update
      Input.update
      update
      break if $scene != self
    end
  end
end

class MessageScene < BaseScene
  def main
    @stage = 0
    @mbox = MsgBoxSprite.new
    @mbox.x = (Graphics.width - 160) / 2
    @mbox.y = 300
    @mbox.bitmap = RPG::Cache.picture(Examples::PICTURE)
    @mbox.bar_bitmap = RPG::Cache.picture(Examples::TITLEBAR)
    @mbox.close_icon = RPG::Cache.icon(Examples::CLOSEICON)
    @mbox.contents.draw_text(0, 0, 132, 26, "Greetings!", 1)
    @mbox.contents.draw_text(4, 32, 160, 24, "It's a nice day!")
    @mbox.contents.draw_text(4, 56, 160, 24, "Isn't it, guys?")
    Graphics.transition
    main_loop
    Graphics.freeze
    @mbox.dispose
  end

  def update
    if @stage == 0
      update_greetings
    else
      update_news
    end
  end

  def update_greetings
    if Input.trigger?(Input::B) or Input.trigger?(Input::C) or
      (Input.left_click? and @mbox.mouse_above_close?)
      Audio.play_cancel
      return $scene = Scene_Title.new
    elsif Input.trigger?(Input::KeyP)
      Audio.play_ok
      @mbox.contents.clear
      @mbox.contents.draw_text(0, 0, 132, 26, "Great News!", 1)
      @mbox.contents.draw_text(4, 32, 160, 24, 'The snapshot was saved!')
      Graphics.save_screenshot
      @stage = 1
    end
  end

  def update_news
    if Input.trigger?(Input::B) or Input.trigger?(Input::C) or
      (Input.left_click? and @mbox.mouse_above_close?)
      Audio.play_cancel
      return $scene = Scene_Title.new
    end
  end
end

class Scene_Example < BaseScene
  include Examples
  def main
    song = RPG::AudioFile.new("The Field of Dreams")
    $game_system.bgm_play(song)
    create_interphase
    Graphics.transition
    entry_animations
    main_loop
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
    Audio.bgm_fade(400)
  end

  def update
    @command_window.update
    if @click_on_window
      @timer -= 1
      @click_on_window = @timer > 0
      unless @click_on_window
        Audio.play_ok
        process_decision
        process_closing
      end
      return
    end
    if Input.trigger?(Input::B)
      Audio.play_cancel
      return $scene = nil
    elsif Input.trigger?(Input::C)
    Audio.play_ok
      process_decision
      return process_closing
    elsif Input.trigger?(Input::MOUSELEFT)
      pos = nil
      3.times{|n| (pos = n; break) if @command_window.mouse_above?(n) }
      return unless pos
      Audio.play_ok
      @click_on_window = true
      @timer = Graphics.frame_rate / 3
      @command_window.index = pos
    end
  end

  def process_decision
    $scene = case @command_window.index
    when 0 then Scene_Title.new
    when 1 then nil
    end
  end

  def process_closing
    @command_window.close
    (@faces + @sprites).each{|s| s.reduce_width! }
  end
end