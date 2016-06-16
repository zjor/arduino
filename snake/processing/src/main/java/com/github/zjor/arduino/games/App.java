package com.github.zjor.arduino.games;

import processing.core.PApplet;
import processing.event.KeyEvent;

public class App extends PApplet {

    private Game game = new Game();
    private boolean gameOver;
    private int blinkCount = 6;

    private long stepDelay;
    private long lastTimestamp;


    @Override
    public void settings() {
        size(480, 480);
    }

    @Override
    public void setup() {
        background(0);
        color(100);

        game.init();
        lastTimestamp = System.currentTimeMillis();
        stepDelay = 500;
    }

    private void render() {
        int w = width / Game.SIZE;
        int h = height / Game.SIZE;

        fill(200, 50, 50);
        for (int i = 0; i < game.getLength(); i++) {
            int x = game.getSnake()[i][0];
            int y = game.getSnake()[i][1];
            rect(x * w, y * h, w, h);
        }
        rect(game.getFood()[0] * w, game.getFood()[1] * h, w, h);
    }

    private void clearScreen() {
        fill(0);
        rect(0, 0, width, height);
    }

    @Override
    public void draw() {
        if (!gameOver) {
            clearScreen();
            render();
            if (System.currentTimeMillis() - lastTimestamp > stepDelay) {
                gameOver = game.advance();
                lastTimestamp = System.currentTimeMillis();
                stepDelay = 500 - 25 * game.getLength();
            }
        } else {
            if (blinkCount == 0) {
                game.init();
                gameOver = false;
                blinkCount = 3;
            } else {
                if (blinkCount % 2 == 0) {
                    render();
                } else {
                    clearScreen();
                }
                blinkCount--;
                delay(250);

            }
        }
    }

    @Override
    public void keyReleased(KeyEvent event) {
        switch (event.getKeyCode()) {
            case 38 /* up */:
                game.up();
                break;
            case 40 /* down */:
                game.down();
                break;
            case 37 /* left */:
                game.left();
                break;
            case 39 /* right */:
                game.right();
                break;
        }
    }

    public static void main(String[] args) {
        PApplet.main(new String[]{"com.github.zjor.arduino.games.App"});
    }
}
