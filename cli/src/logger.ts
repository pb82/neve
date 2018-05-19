import {Logger, transports} from "winston";

const LoggerConfig = {
    console: {
        colorize: true,
        enabled: true,
        level: "info",
    },
};

const logger = new Logger({
    // We only need to log to console
    console: {
        colors: {
            debug: "blue",
            error: "red",
            info: "green",
            warn: "yellow",
        },
    },
});

if (LoggerConfig.console.enabled) {
    logger.add(transports.Console, {
        colorize: LoggerConfig.console.colorize,
        level: LoggerConfig.console.level,
    });
}

export const info = logger.info;
export const warn = logger.warn;
export const error = logger.error;
export const debug = logger.debug;
export function setLogLevel(level: string): void {
    logger.transports.console.level = level;
}