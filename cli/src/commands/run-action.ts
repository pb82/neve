import axios from "axios";
import {readFileSync, statSync} from "fs";

function readFileAsJSON(path: string) : string {
    const contents: Buffer = readFileSync(path);
    return JSON.parse(contents.toString());
}

export default function runAction(name: string, options: any) {
    let url = options.url + "/action/" + name;

    if (options.block) {
        url += "?block=true";
    }

    let payload: any = {};

    if (options.file && statSync(options.file).isFile()) {
        payload = readFileAsJSON(options.file);
    }

    return axios.post(url, payload);
}
