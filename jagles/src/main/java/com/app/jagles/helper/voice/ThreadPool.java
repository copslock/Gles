
package com.app.jagles.helper.voice;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public final class ThreadPool {

    public static void initialize() {
        executor = new ThreadPoolExecutor(2, 2, 60, TimeUnit.SECONDS,
                new LinkedBlockingQueue<Runnable>());
    }

    private static ThreadPoolExecutor executor;

    public static ThreadPoolExecutor getThreadPoolExecutor() {
        return executor;
    }


    /**
     * @param runnable
     */
    public static void execute(Runnable runnable) {
        executor.execute(runnable);
    }
}
