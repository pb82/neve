import axios from "axios";
import {readFileSync, statSync} from "fs";

function readFileAsJSON(path: string) : string {
    const contents: Buffer = readFileSync(path);
    return JSON.parse(contents.toString());
}

export default function runAction(name: string, options: any) {
    const url = options.url + "/action/" + name;

    let payload = null;

    if (options.file && statSync(options.file).isFile()) {
        payload = readFileAsJSON(options.file);
    }

    return axios.post(url, payload);
}