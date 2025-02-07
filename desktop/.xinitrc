source ~/.bashrc
xrdb -merge ~/.Xresources
~/window/bin/test &        # バックグラウンドで実行
conky -c ~/.conky/.conkyrc/conky.config &   # バックグラウンドで実行
exec kitty               # xtermを最終的に実行

