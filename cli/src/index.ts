import {readFileSync, statSync} from "fs";
import {join} from "path";
import {Config, CONFIG_FILE_NAME} from "./config";
import {homedir} from "os";
import {CommanderStatic} from "commander";
import {error, info} from "./logger";
import * as program from "commander";
import ping from "./commands/ping";
import connect from "./commands/connect";
import createAction from "./commands/create-action";
import {AxiosError} from "axios";

function logSuccess(result: any) {
    info("Operation successful");
    if (result.data) {
        info(result.data);
    }
}

function logError(err: AxiosError) {
    error(err.toString());
    if (err.response && err.response.data) {
        error(err.response.data);
    }
}

function tryReadConfig() : Config {
    const configPath = join(homedir(), CONFIG_FILE_NAME);
    try {
        const stats = statSync(configPath);
        if (stats.isFile()) {
            const contents: Buffer = readFileSync(configPath);
            return new Config(contents.toString());
        } else {
            return null;
        }
    } catch {
        return null;
    }
}

function main() {
    const config = (program: CommanderStatic) => {
        const c: Config = tryReadConfig();
        if (!c) throw new Error("Please connect to server first");
        return {...c, ...program};
    };

    program
        .version("CLI version: " + require("../package.json").version);

    program
        .option("-f --file <file>", "File to upload");

    program
        .command("ping")
        .description("Check server connectivity")
        .action(() => {
            ping(config(program).url)
                .then(logSuccess)
                .catch(logError);
        });

    program
        .command("connect <url>")
        .description("Connect to server")
        .action(url => {
            connect(url);
        });

    program
        .command("create-action <name>")
        .description("Create a new action")
        .action(name => {
            createAction(name, config(program))
                .then(logSuccess)
                .catch(logError);
        });

    program.parse(process.argv);
}

try {
    main();
} catch (err) {
    error("Error running command: %s", err.message);
}