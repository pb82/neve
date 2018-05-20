import {readFileSync, statSync} from "fs";
import axios from "axios";

function readFileAsBase64(path: string) : string {
    const contents: Buffer = readFileSync(path);
    return contents.toString("base64");
}

export default function createAction(name: string, options: any) {
    const url = options.url + "/action";

    if (!options.file) {
        throw new Error("File argument missing");
    }

    if (!statSync(options.file).isFile()) {
        throw new Error("File not found: " + options.file);
    }

    const payload = {
        name,
        timeout: Math.abs(parseInt(options.timeout)) || 0,
        memory: Math.abs(parseInt(options.memory)) || 0,
        code: readFileAsBase64(options.file)
    };

    return axios.post(url, payload);
}