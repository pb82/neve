import {error, info} from "./logger";
import {match} from "missmatch";
import connect from "./commands/connect";
import {readFileSync, statSync} from "fs";
import {join} from "path";
import {Config, CONFIG_FILE_NAME} from "./config";
import {homedir} from "os";
import ping from "./commands/ping";

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

function removeOptions(args: Array<string>): Array<string> {
    return args.filter(arg => {
        return !(arg.startsWith("-") || arg.startsWith("--"));
    });
}

function main() {
    const config: Config = tryReadConfig();
    const args = removeOptions(process.argv);

    // Commands that can run without the server being configured
    if(!match(args, {
        'a(_,_,s("connect"),s@url)': ({url} : {url: string}) => connect(url),
        '_': true
    }, {arrow: true})) {
        return;
    }

    if (!config) {
        error("Not connected. Please run the `connect <url>` command first");
        return;
    }

    // Commands that require configuration
    match(args, {
        'a(_,_,s("ping"))': () => { return ping(config.url); },
        '_': () => { throw new Error("Unrecognized command") }
    }, {arrow: true}).then(() => {
        info("Command successful");
    }).catch((err: any) => {
        error(err);
    });
}

try {
    main();
} catch (err) {
    error("Error running command: %s", err.message);
}