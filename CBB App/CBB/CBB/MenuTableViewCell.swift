//
//  MenuTableViewCell.swift
//  CBB
//
//  Created by Erich Chu on 4/18/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit

class MenuTableViewCell: UITableViewCell {

    @IBOutlet weak var menuCellLine: UIView!
    
    @IBOutlet weak var menuCellView: UIView!
    @IBOutlet weak var menuLabel: UILabel!
    
    override func awakeFromNib() {
        super.awakeFromNib()
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
