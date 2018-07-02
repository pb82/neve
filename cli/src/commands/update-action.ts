import {readFileSync, statSync} from "fs";
import axios from "axios";

function readFileAsBase64(path: string) : string {
    const contents: Buffer = readFileSync(path);
    return contents.toString("base64");
}

export default function updateAction(name: string, options: any) {
    const url = options.url + "/action/" + name;
    let updateCode = false;

    if (options.file && statSync(options.file).isFile()) {
        updateCode = true;
    }

    const payload: any = {name};
    if (options.timeout !== undefined) payload.timeout = Math.abs(parseInt(options.timeout));
    if (options.memory !== undefined) payload.memory = Math.abs(parseInt(options.memory));
    if (updateCode) payload.code = readFileAsBase64(options.file);

    return axios.put(url, payload);
}
