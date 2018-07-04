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
import listActions from "./commands/list-actions";
import runAction from "./commands/run-action";
import getAction from "./commands/get-action";
import deleteAction from "./commands/delete-action";
import updateAction from "./commands/update-action";

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
        .option("-f --file <file>", "File to upload")
        .option("-t --timeout <timeout>", "Time limit (ms)")
        .option("-m --memory <memory>", "Memory limit (kb)")
        .option("-b --block", "Wait for result");

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

    program
        .command("update-action <name>")
        .description("Update existint action")
        .action(name => {
            updateAction(name, config(program))
                .then(logSuccess)
                .catch(logError);
        });

    program
        .command("list-actions")
        .description("List available actions")
        .action(() => {
            listActions(config(program))
                .then(logSuccess)
                .catch(logError)
        });

    program
        .command("run-action <name>")
        .description("Run action")
        .action(name => {
            runAction(name, config(program))
                .then(logSuccess)
                .catch(logError);
        });

    program
        .command("get-action <name>")
        .description("Get action details")
        .action(name => {
            getAction(name, config(program))
                .then(logSuccess)
                .catch(logError);
        });

    program
        .command("delete-action <name>")
        .description("Delete action")
        .action(name => {
            deleteAction(name, config(program))
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
