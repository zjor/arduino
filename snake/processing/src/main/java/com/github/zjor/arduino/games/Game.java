package com.github.zjor.arduino.games;

import java.util.Random;

public class Game {

    public static final int SIZE = 8;

    private int[][] snake = new int[SIZE * SIZE][2];
    private int length;
    private int[] food = new int[2];
    private int[] v = new int[2];

    public void init() {
        int half = SIZE / 2;
        length = SIZE / 3;

        for (int i = 0; i < length; i++) {
            snake[i][0] = half - 1;
            snake[i][1] = half + i;
        }

        food[0] = half - 1;
        food[1] = half - 1;

        v[0] = 0;
        v[1] = -1;

    }

    /**
     * @return true if game is over
     */
    public boolean advance() {
        int[] head = new int[]{snake[0][0] + v[0], snake[0][1] + v[1]};

        if (head[0] < 0 || head[0] >= SIZE) {
            return true;
        }

        if (head[1] < 0 || head[1] >= SIZE) {
            return true;
        }

        for (int i = 0; i < length; i++) {
            if (snake[i][0] == head[0] && snake[i][1] == head[1]) {
                return true;
            }
        }

        boolean grow = (head[0] == food[0] && head[1] == food[1]);
        if (grow) {
            length++;
            Random rand = new Random(System.currentTimeMillis());
            food[0] = rand.nextInt(SIZE);
            food[1] = rand.nextInt(SIZE);
        }

        for (int i = length - 1; i >= 0; i--) {
            snake[i + 1][0] = snake[i][0];
            snake[i + 1][1] = snake[i][1];
        }
        snake[0][0] += v[0];
        snake[0][1] += v[1];
        return false;
    }

    public void up() {
        if (v[0] != 0) {
            v[0] = 0;
            v[1] = -1;
        }
    }

    public void down() {
        if (v[0] != 0) {
            v[0] = 0;
            v[1] = 1;
        }
    }

    public void left() {
        if (v[1] != 0) {
            v[0] = -1;
            v[1] = 0;
        }
    }

    public void right() {
        if (v[1] != 0) {
            v[0] = 1;
            v[1] = 0;
        }
    }

    public int[][] getSnake() {
        return snake;
    }

    public int getLength() {
        return length;
    }

    public int[] getFood() {
        return food;
    }
}
