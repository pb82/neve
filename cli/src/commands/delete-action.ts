import axios from "axios";

export default function deleteAction(name: string, options: any) {
    const url = options.url + "/action/" + name;
    return axios.delete(url);
}
