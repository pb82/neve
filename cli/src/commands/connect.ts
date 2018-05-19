import {error, info} from "../logger";
import ping from "./ping";
import {writeFileSync} from "fs";
import {join} from "path";
import {homedir} from "os";
import {CONFIG_FILE_NAME} from "../config";

const CONFIG_PATH = join(homedir(), CONFIG_FILE_NAME);

function updateOrCreateConfig(url: string) {
    const config = {url};
    writeFileSync(CONFIG_PATH, JSON.stringify(config));
}

export default function connect(url: string) {
    ping(url)
        .then(() => {
            updateOrCreateConfig(url);
            info("Config written to %s", CONFIG_PATH);
        })
        .catch((err) => {
            error(err);
        });
}